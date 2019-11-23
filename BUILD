cc_library(
    name = "transit",
    srcs = [
        "src/transit_client.cpp",
    ],
    hdrs = [
        "include/transit_client.h",
        "include/types.h",
    ],
    strip_include_prefix="include",
    deps = [
        ":util",
    ]
)

cc_library(
    name = "transit_plugin",
    srcs = [
        "src/transit_plugin.cpp"
    ],
    hdrs = [
        "include/transit_plugin.h"
    ],
    strip_include_prefix="include",
    deps = [
        ":transit",
        ":libsignboard"
    ]
)

cc_library(
    name = "weather_plugin",
    srcs = [
         "src/weather_plugin.cpp"
    ],
    hdrs = [
         "include/weather_plugin.h",
    ],
    strip_include_prefix="include",
    deps = [
         "@json//:json",
         ":libsignboard",
         ":util",
    ]
)

cc_library(
    name = "news_plugin",
    srcs = [
         "src/news_plugin.cpp"
    ],
    hdrs = [
         "include/news_plugin.h",
    ],
    strip_include_prefix="include",
    deps = [
         "@json//:json",
         ":libsignboard",
         ":util",
    ]
)

cc_library(
    name = "mux_plugin",
    srcs = [
        "src/mux_plugin.cpp",
    ],
    hdrs = [
        "include/mux_plugin.h",
    ],
    strip_include_prefix= "include",
    deps = [
        ":libsignboard"
    ]
)

cc_library(
    name = "util",
    srcs = [
        "src/deadpool.cpp",
    ],
    hdrs = [
        "include/deadpool.h",
        "include/rapidxml.hpp",
        "include/util.h",
    ],
    strip_include_prefix="include",
    deps = [
        "@abseil//absl/hash:hash",
        "@curl//:curl",
    ],
)

cc_library(
    name = "libsignboard",
    srcs = [
        "src/animation.cpp",
        "src/graphics_util.cpp",
    ],
    hdrs = [
        "include/animation.h",
        "include/graphics_util.h",
        "include/signboard_plugin.h",
    ],
    strip_include_prefix="include",
    deps = [
        "@librgbmatrix//:librgbmatrix",
        ":util",
    ],
)

cc_test(
    name = "test_transit",
    srcs = [
        "test/test_transit_client.cpp",
        "test/test_util.cpp",
    ],
    deps = [
        ":transit",
        "@gtest//:gtest",
    ]
)

cc_test(
    name = "test_weather",
    srcs = [
         "test/test_weather.cpp",
    ],
    deps = [
        ":weather_plugin",
        "@gtest//:gtest",
    ]
)

cc_binary(
    name = "signboard",
    srcs = ["src/signboard.cpp"],
    deps = [
        ":libsignboard",
        ":mux_plugin",
        ":news_plugin",
        ":transit_plugin",
        ":weather_plugin",
    ]
)
