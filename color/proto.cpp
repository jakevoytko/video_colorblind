/**
 * Contains the implementation of the protanope conversion.
 *
 * Copyright 2016 Jacob Voytko (jakevoytko@gmail.com)
 */


#include "proto.hpp"

#include <math.h>
#include <algorithm>

#include <boost/scoped_ptr.hpp>

#include "color.hpp"

using video_colorblind::color::Rgb;
using video_colorblind::color::Xyy;
using video_colorblind::color::Xyz;


/** The protanope confusion point in the xyY spectrum. */
Xyy xyyConfusionPoint = Xyy(0.747, 0.253, 1.0);


/** Stores the coefficients of a quadratic polynomial. */
struct QuadraticPolynomial {
  QuadraticPolynomial(double xSquare_, double x_, double c_):
      xSquare(xSquare_), x(x_), c(c_) {}

  const double xSquare;
  const double x;
  const double c;
};


/** Calculates a step of the LaGrange interpolation method. */
QuadraticPolynomial lagrangeStepXyy(
    const Xyy &xyy0, const Xyy &xyy1, const Xyy &xyy2) {
  const double xSquare = xyy0.y / ((xyy0.x - xyy1.x) * (xyy0.x - xyy2.x));
  const double x = xSquare * -xyy1.x + xSquare * -xyy2.x;
  const double c = xSquare * xyy1.x * xyy2.x;
  return QuadraticPolynomial(xSquare, x, c);
}


/** Interpolates through the three points given using Lagrange interpolation. */
QuadraticPolynomial lagrangeInterpolateXyy(
    const Xyy &xyy0, const Xyy &xyy1, const Xyy &xyy2) {
  QuadraticPolynomial result0 = lagrangeStepXyy(xyy0, xyy1, xyy2);
  QuadraticPolynomial result1 = lagrangeStepXyy(xyy1, xyy0, xyy2);
  QuadraticPolynomial result2 = lagrangeStepXyy(xyy2, xyy0, xyy1);

  return QuadraticPolynomial(
    result0.xSquare + result1.xSquare + result2.xSquare,
    result0.x + result1.x + result2.x,
    result0.c + result1.c + result2.c);
}


/**
 * Protanopes can see wavelengths 470 and 575 correctly, as well as white. Doing
 * polynomial interpolation between these points produces a polynomial that can
 * be used to estimate all colors that protanopes can see.
 */
QuadraticPolynomial xyyVisionCurve = lagrangeInterpolateXyy(
  XYY_470, XYY_575, XYY_WHITE_D50);


/** Represents a line. */
struct Line {
  Line(double m_, double b_): m(m_), b(b_) {}

  const double m;
  const double b;
};


/** Calculates a line between the two given xyY coordinates. */
Line xyyLine(const Xyy &xyy0, const Xyy &xyy1) {
  // The confusion point for protanopes is outside the sRGB colorspace, so this
  // can't divide-by-0.

  if (xyy0.x > xyy1.x) {
    return xyyLine(xyy1, xyy0);
  }
  double slope = (xyy1.y - xyy0.y) / (xyy1.x - xyy0.x);
  return Line(slope, xyy0.y - slope * xyy0.x);
}


/**
 * Protans have a dark response curve at the red end of the spectrum. Some reds
 * are perceived at ~1/10 of the intensity that a normal observer would
 * see. Adjust for this.
 */
double getProtanLuminance(const Xyz &xyz) {
  return -0.460 * xyz.x + 1.359 * xyz.y + 0.101 * xyz.z;
}


/** 
 * Calculates the intersection between a quadratic and a line using the
 * ::drumroll:: quadratic formula!
 */
Xyy intersectCurveLineXyy(
    const QuadraticPolynomial &poly, const Line &line, double Y) {
  double A = poly.xSquare;
  double B = poly.x - line.m;
  double C = poly.c - line.b;
  double discriminant = B*B - 4.0 * A * C;

  // Only the smaller X root falls in the color space.
  assert(discriminant >= 0);

  double x = (-B + sqrt(discriminant)) / (2.0 * A);

  return Xyy(x, line.m * x + line.b, Y);
}


/** 
 * If the xyy color falls outside the triangle defined by the sRGB primaries,
 * does a best-effort to move it back within the sRGB triangle by intersecting
 * the confusion line with the sRGB triangle.
 */
Xyy moveWithinRgb(const Line &line, const Xyy &xyy) {
  // Two cases for protanopes: line intersects Blue->Green, line intersects
  // Red->Green.
  Line primaryLine = xyy.x < XYY_GREEN_PRIMARY.x ?
        xyyLine(XYY_BLUE_PRIMARY, XYY_GREEN_PRIMARY) :
        xyyLine(XYY_GREEN_PRIMARY, XYY_RED_PRIMARY);

  double y = primaryLine.m * xyy.x + primaryLine.b;
  if (xyy.y > y) {
    double newX = (primaryLine.b - line.b) / (line.m - primaryLine.m);
    double newY = primaryLine.m * newX + primaryLine.b;
    return Xyy(newX, newY, xyy.Y);
  }

  return xyy;
}


/** Returns x if within min or max, otherwise the violated bound. */
double clamp(double min, double x, double max) {
  return std::max(min, std::min(x, max));
}


Rgb video_colorblind::color::getProtoColor(const Rgb &inputRgb) {
  Xyz xyz = rgbToXyz(inputRgb);
  Xyy xyy = xyzToXyy(xyz);

  // According to
  // http://nvlpubs.nist.gov/nistpubs/jres/33/jresv33n6p407_A1b.pdf, luminance
  // for protans must be adjusted because of the weak red frequency response.
  double protanLuminance = getProtanLuminance(xyz);

  // First, find the confusion line. All colors along this line are perceived as
  // identical to protanopes.
  Line confusionLine = xyyLine(xyyConfusionPoint, xyy);

  // The intersection between the vision curve and the confusion line is an
  // estimation of the color a protanope actually sees.
  Xyy xyyIntersection = intersectCurveLineXyy(
    xyyVisionCurve, confusionLine, protanLuminance);

  // The color may have fallen outside the sRGB colorspace. If so, move it back
  // along the confusion line. Note: This can still convert to colors outside of
  // the display sRGB colorspace.
  Xyy boundedXyy = moveWithinRgb(confusionLine, xyyIntersection);

  // Convert back to RGB.
  Rgb returnRgb = xyzToRgb(xyyToXyz(boundedXyy));

  // Unclear what to do here besides clamp. If this is outside [0-255], then
  // it's the case where there was no intersection between the confusion line
  // and the sRGB colorspace.
  return Rgb(
    clamp(0, round(returnRgb.r), 255),
    clamp(0, round(returnRgb.g), 255),
    clamp(0, round(returnRgb.b), 255));
}
