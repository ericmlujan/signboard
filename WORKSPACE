workspace(name = "signboard")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

# local_repository(
#   name = "librgbmatrix",
#   path = "external/librgbmatrix",
# )

http_archive(
    name="librgbmatrix",
    urls=["https://github.com/hzeller/rpi-rgb-led-matrix/archive/4c9a4036f44f173b783a0ba701138605f8b7d95c.zip"],
    strip_prefix="rpi-rgb-led-matrix-4c9a4036f44f173b783a0ba701138605f8b7d95c",
    sha256="162f5c7c9a85cbaf4a89db9ba23f8e5b4e975223039755e222d3092a7cd0cb97",
    build_file="//external:librgbmatrix.BUILD"
)

http_archive(
  name = "abseil",
  urls = ["https://github.com/abseil/abseil-cpp/archive/8f11724067248acc330b4d1f12f0c76d03f2cfb1.zip"],
  strip_prefix = "abseil-cpp-8f11724067248acc330b4d1f12f0c76d03f2cfb1",
  sha256 = "cdbfcf4e5d2f61273325a90392ffc224469daf0800e5753ce48032db06d250ee"
)

http_archive(
    name = "gtest",
    urls = ["https://github.com/google/googletest/archive/e110929a7b496714c1f6f6be2edcf494a18e5676.zip"],
    strip_prefix = "googletest-e110929a7b496714c1f6f6be2edcf494a18e5676",
    sha256 = "68f4935be244a098adf3ca4ee1a06ce1cf5d14cda3e79bd1ca3a526628f1d978"
)

http_archive(
    name = "json",
    urls = ["https://github.com/nlohmann/json/archive/v3.7.0.zip"],
    strip_prefix = "json-3.7.0",
    sha256 = "37b25617dccb359366d8ac75912d9a25e20fe15d0ed942819e23d02003d39f87",
    build_file="//external:json.BUILD"
)

new_local_repository(
  name = "curl",
  path = "",
  build_file = "//external:curl.BUILD",
)
