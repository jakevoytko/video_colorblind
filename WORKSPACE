new_local_repository(
    name = "opencv",
    path = "/usr/local/",
    build_file = "opencv.BUILD",
)

new_local_repository(
  name = "boost",
  path = "/usr/local/Cellar/boost/1.60.0_1",
  build_file = "boost.BUILD",
)

new_http_archive(
    name = "gtest",
    url = "https://googletest.googlecode.com/files/gtest-1.7.0.zip",
    sha256 = "247ca18dd83f53deb1328be17e4b1be31514cedfc1e3424f672bf11fd7e0d60d",
    build_file = "gtest.BUILD",
    strip_prefix = "gtest-1.7.0",
)
