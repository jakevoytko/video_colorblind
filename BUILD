package(default_visibility=["//visibility:public"])

cc_binary (
  name = "video_colorblind",
  srcs = glob(["*.cpp"], exclude=["*_test.cpp", "_test.hpp"]),
  copts = ["-g"],
  linkopts = ["-lprofiler"],
  deps = [
    "@boost//:boost",
    "@opencv//:opencv",
    "//color:color",
  ],
)

test_suite (
  name = "all_tests",
  tests = [
    "//color:all_tests",
  ],
)