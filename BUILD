cc_library(
    name = "transit",
    copts = ["-O3", "-g", "-Wall", "-Wextra"],
    srcs = [
        "src/deadpool.cpp",
        "src/transit_client.cpp",
    ],
    hdrs = [
        "include/deadpool.h",
        "include/rapidxml.hpp",
        "include/transit_client.h",
        "include/types.h",
        "include/util.h",
    ],
    strip_include_prefix="include",
    deps = [
        "@abseil//absl/container:flat_hash_map",
        "@abseil//absl/container:flat_hash_set",
        "@abseil//absl/hash",
        "@curl//:curl",
    ]
)

cc_library(
    name = "libsignboard",
    copts = ["-O3", "-g", "-Wall", "-Wextra"],
    srcs = [
        "src/graphics_util.cpp",
        "src/pager.cpp",
    ],
    hdrs = [
        "include/graphics_util.h",
        "include/pager.h",
        "include/signboard_plugin.h",
    ],
    strip_include_prefix="include",
    deps = [
        ":transit",
        "@librgbmatrix//:librgbmatrix",
    ],
)

cc_test(
    name = "test_transit",
    copts = ["-O0", "-g", "-Wall", "-Wextra"],
    srcs = [
        "test/test_transit_client.cpp",
        "test/test_util.cpp",
    ],
    deps = [
        ":transit",
        "@gtest//:gtest",
    ]
)

cc_binary(
    name = "signboard",
    srcs = ["src/signboard.cpp"],
    deps = [":libsignboard"]
)
