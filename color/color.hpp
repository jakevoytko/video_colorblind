/**
 * Contains the declaration of color classes and functions.
 *
 * Copyright 2016 Jacob Voytko (jakevoytko@gmail.com)
 */

#ifndef __VIDEO_COLORBLIND_COLOR_COLOR_HPP__
#define __VIDEO_COLORBLIND_COLOR_COLOR_HPP__

#include <math.h>

#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>

namespace video_colorblind { namespace color {


// Rgb is an immutable RGB color with intensities between 0 and 255. Note that
// values can move outside of this range when converted from other color spaces,
// since not every color is representable in RGB.
struct Rgb {
  Rgb(double r_, double g_,  double b_): r(r_), g(g_), b(b_) {}

  const double r;
  const double g;
  const double b;
};


// Xyy is an immutable XYY color.
struct Xyy {
  Xyy(double x_, double y_,  double Y_): x(x_), y(y_), Y(Y_) {}

  const double x;
  const double y;
  const double Y;
};


// Xyz is an immutable Xyz color.
struct Xyz {
  Xyz(double x_, double y_, double z_): x(x_), y(y_), z(z_) {}

  const double x;
  const double y;
  const double z;
};


Xyz rgbToXyz(const Rgb &rgb);
Xyz xyyToXyz(const Xyy &xyy);
Rgb xyzToRgb(const Xyz &xyz);


// xyzToXyy converts the given xyz coordinate to the xyY coordinate. It handles
// zero-divide by setting the result to zero.
Xyy xyzToXyy(const Xyz &xyz);


// The sRGB primaries in xyY. Calculated using RGB(255, 0, 0) and the conversion
// functions defined here.
const Xyy XYY_RED_PRIMARY(0.648427223687212, 0.33085610147277805, 0.2225045);


// The sRGB primaries in xyY. Calculated using RGB(0, 255, 0) and the conversion
// functions defined here.
const Xyy XYY_GREEN_PRIMARY(0.32114218947031314, 0.5978731460291832, 0.7168786);


// The sRGB primaries in xyY. Calculated using RGB(0, 0, 255) and the conversion
// functions defined here.
const Xyy XYY_BLUE_PRIMARY(0.15588297522548386, 0.06604079049922723, 0.0606169);


// The D50 white point in xyY.
const Xyy XYY_WHITE_D50(0.3457, 0.3585, 1);


// The 470 wavelength in xyY. Taken from CIE publication 15.
const Xyy XYY_470 = Xyy(0.12412, 0.05780, 0.090980);


// The 575 wavelength in xyY. Taken from CIE publication 15.
const Xyy XYY_575 = Xyy(0.47877, 0.52020, 0.915400);


// srgbToLinearGamma converts between nonlinear srgb channels, and linear RGB
// values.
double srgbToLinearGamma(double intensity);


// linearGammaToSrgb converts a linear intensity rgb channel to the srgb gamma.
double linearToSrgbGamma(double intensity);


}  // namespace color
}  // namespace video_colorblind

#endif
