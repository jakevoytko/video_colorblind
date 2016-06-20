/**
 * Contains the implementation for color functions.
 *
 * Copyright 2016 Jacob Voytko (jakevoytko@gmail.com)
 */

#include <math.h>

#include "color.hpp"

using video_colorblind::color::Rgb;
using video_colorblind::color::Xyy;
using video_colorblind::color::Xyz;


Xyy video_colorblind::color::xyzToXyy(const Xyz &xyz) {
  double sum = xyz.x + xyz.y + xyz.z;
  double finalX = XYY_WHITE_D50.x;
  double finalY = XYY_WHITE_D50.y;

  if (sum != 0) {
    finalX = xyz.x / sum;
    finalY = xyz.y / sum;
  }

  return Xyy(finalX, finalY, xyz.y);
}


Xyz video_colorblind::color::xyyToXyz(const Xyy &xyy) {
  double finalX = 0;
  double finalY = 0;
  double finalZ = 0;

  if (xyy.y != 0) {
    finalX = xyy.x * xyy.Y / xyy.y;
    finalY = xyy.Y;
    finalZ = (1. - xyy.x - xyy.y) * xyy.Y / xyy.y;
  }
  return Xyz(finalX, finalY, finalZ);
}


/** 
 * Converts an RGB color into an equivalent color in the XYZ color space. RGB
 * uses D65 as white, and XYZ uses D50 as white, so it uses a Bradford chromatic
 * adaptation matrix gotten from
 * http://www.brucelindbloom.com/index.html?Eqn_RGB_XYZ_Matrix.html.
 */
Xyz video_colorblind::color::rgbToXyz(const Rgb &rgb) {
  double r = srgbToLinearGamma(rgb.r / 255);
  double g = srgbToLinearGamma(rgb.g / 255);
  double b = srgbToLinearGamma(rgb.b / 255);

  // The RGB-to-XYZ matrix with a Bradford-adapted D65->D50 adjustment.
  double X = r * 0.4360747 + g * 0.3850649 + b * 0.1430804;
  double Y = r * 0.2225045 + g * 0.7168786 + b * 0.0606169;
  double Z = r * 0.0139322 + g * 0.0971045 + b * 0.7141733;

  return Xyz(X, Y, Z);
}


/**
 * Gamma-corrects a sRGB intensity. Taken from 
 * http://www.w3.org/Graphics/Color/srgb 
 */
double video_colorblind::color::srgbToLinearGamma(double intensity) {
  assert(intensity >= 0);
  assert(intensity <= 1);

  if (intensity < 0.04045) {
    return intensity / 12.92;
  }

  double alpha = .055;
  return pow((intensity + alpha) / (1 + alpha), 2.4);
}


/** 
 * Gamma-corrects an RGB value that's been converted from XYZ
 * intensity. Inferred from http://www.w3.org/Graphics/Color/srgb
 */
double video_colorblind::color::linearToSrgbGamma(double intensity) {
  if (intensity <= 0.0031308) {
    return intensity * 12.92;
  }

  double alpha = .055;
  return (1 + alpha) * pow(intensity, 1 / 2.4) - alpha;
}


/** Converts an XYZ color into an equivalent RGB color. */
Rgb video_colorblind::color::xyzToRgb(const Xyz &xyz) {
  // The XYZ-to-RGB matrix with a Bradford-adapted D65->D50 adjustment.
  double newR = xyz.x * 3.1338561  + xyz.y * -1.6168667 + xyz.z * -0.4906146;
  double newG = xyz.x * -0.9787684 + xyz.y * 1.9161415  + xyz.z * 0.0334540;
  double newB = xyz.x * 0.0719453  + xyz.y * -0.2289914 + xyz.z * 1.4052427;

  double finalR = linearToSrgbGamma(newR);
  double finalG = linearToSrgbGamma(newG);
  double finalB = linearToSrgbGamma(newB);

  return Rgb(255 * finalR, 255 * finalG, 255 * finalB);
}
