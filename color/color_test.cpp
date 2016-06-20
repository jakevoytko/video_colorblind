/**
 * Contains tests for the color package.
 *
 * Copyright 2016 Jacob Voytko (jakevoytko@gmail.com)
 */


#include <gtest/gtest.h>

#include "color.hpp"
#include "proto.hpp"


using video_colorblind::color::Rgb;
using video_colorblind::color::Xyy;
using video_colorblind::color::Xyz;


void assertRgbRoundTrip(const Rgb &rgb) {
  Rgb newRgb =
      xyzToRgb(
          xyyToXyz(
              xyzToXyy(
                  rgbToXyz(rgb))));

  EXPECT_NEAR(rgb.r, newRgb.r, .01);
  EXPECT_NEAR(rgb.g, newRgb.g, .01);
  EXPECT_NEAR(rgb.b, newRgb.b, .01);
}


TEST(VIDEO_COLORBLIND_COLOR, roundtrip_test) {
  // Incrementing by 5 is a reasonable compromise.
  for (int r = 0; r <= 255; r+=5) {
    for (int g = 0; g <= 255; g+=5) {
      for (int b = 0; b <= 255; b+=5) {
        assertRgbRoundTrip(Rgb(r, g, b));
      }
    }
  }
}


TEST(VIDEO_COLORBLIND_COLOR, red_primary) {
  Rgb rgb(255, 0, 0);
  Xyy xyy = xyzToXyy(rgbToXyz(rgb));
  EXPECT_NEAR(.64, xyy.x, .01);
  EXPECT_NEAR(.33, xyy.y, .01);
}


TEST(VIDEO_COLORBLIND_COLOR, green_primary) {
  Rgb rgb(0, 255, 0);
  Xyy xyy = xyzToXyy(rgbToXyz(rgb));
  EXPECT_NEAR(.30, xyy.x, .05);
  EXPECT_NEAR(.60, xyy.y, .05);
}


TEST(VIDEO_COLORBLIND_COLOR, blue_primary) {
  Rgb rgb(0, 0, 255);
  Xyy xyy = xyzToXyy(rgbToXyz(rgb));
  EXPECT_NEAR(.15, xyy.x, .01);
  EXPECT_NEAR(.06, xyy.y, .01);
}


TEST(VIDEO_COLORBLIND_COLOR, white_proto) {
  Rgb white(255, 255, 255);
  Rgb protoWhite(white);
  EXPECT_EQ(white.r, protoWhite.r);
  EXPECT_EQ(white.g, protoWhite.g);
  EXPECT_EQ(white.b, protoWhite.b);
}


TEST(VIDEO_COLORBLIND_COLOR, black_proto) {
  Rgb black(0, 0, 0);
  Rgb protoBlack(black);
  EXPECT_EQ(black.r, protoBlack.r);
  EXPECT_EQ(black.g, protoBlack.g);
  EXPECT_EQ(black.b, protoBlack.b);
}


TEST(VIDEO_COLORBLIND_COLOR, proto_flat_confusion_line) {
  Rgb rgb = xyzToRgb(xyyToXyz(Xyy(.3, .253, .5)));

  // Blue falls outside the bounds of this color.
  EXPECT_GT(rgb.b, 255);
  Rgb clippedRgb = Rgb(rgb.r, rgb.g, 255);
  Rgb value = getProtoColor(clippedRgb);

  Rgb expected(165, 182, 255);

  EXPECT_NEAR(value.r, expected.r, .5);
  EXPECT_NEAR(value.g, expected.g, .5);
  EXPECT_NEAR(value.b, expected.b, .5);
}


int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
