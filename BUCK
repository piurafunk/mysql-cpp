load("subdir_glob.bzl", "subdir_glob")

cxx_library(
    name = "mysql-cpp",
    compiler_flags = [
        '-std=c++2a',
    ],
    header_namespace = "MysqlCpp",
    exported_headers = subdir_glob([
        ("include", "**/*.h"),
        ("include", "**/*.hpp"),
    ]),
    headers = subdir_glob([
      ('private_include', '**/*.hpp'),
      ('private_include', '**/*.h'),
    ]),
    srcs = glob([
        "src/**/*.c",
        "src/**/*.cpp",
    ]),
    link_style = "static",
    exported_deps = [
        "//.conan2/p/opens82d4fa30da96c/p:openssl",
        "//.conan2/p/zlib10e4879147c91/p:zlib",
        "//.conan2/p/catch95371a689a8ee/p:catch2",
    ],
)

cxx_binary(
    name = 'test',
    srcs = glob([
        'test/**/*.cpp',
    ]),
    deps = [
        ':mysql-cpp',
    ],
)
