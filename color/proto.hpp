/**
 * Contains the declaration of the protanope conversion.
 *
 * Copyright 2016 Jacob Voytko (jakevoytko@gmail.com)
 */


#ifndef __VIDEO_COLORBLIND_COLOR_PROTO_HPP__
#define __VIDEO_COLORBLIND_COLOR_PROTO_HPP__

namespace video_colorblind { namespace color {

// getProtoColor takes an RGB color, and calculates an estimate for the color as
// perceived by a protanope.
Rgb getProtoColor(const Rgb &inputRgb);

}  // namespace color
}  // namespace video_colorblind

#endif
