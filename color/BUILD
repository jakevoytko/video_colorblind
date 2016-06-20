package(default_visibility=["//visibility:public"])

cc_library (
  name = "color",
  hdrs = glob(["*.hpp"], exclude=["*_test.hpp"]),
  srcs = glob(["*.cpp"], exclude=["*_test.cpp"]),
  copts = ["-g"],
  deps = [
    "@boost//:boost",
  ],
)

cc_test (
  name = "test",
  srcs = glob(["color_test.cpp"]),
  copts = ["-Iexternal/gtest/include"],
  deps = [
      ":color",
    "@gtest//:main",
  ],
)

test_suite (
  name = "all_tests",
  tests = [
    ":test",
  ],
)