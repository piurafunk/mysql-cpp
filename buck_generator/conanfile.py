from conan import ConanFile
from conan.tools.files import save
from textwrap import dedent
import os
import re

# Debug
from pprint import pprint, pformat

INDENT = '    '

class BuckGenerator:
    def __init__(self, conanfile):
        self._conanfile = conanfile

    def generate(self):
        buck_file = dedent(f"""\
        load("subdir_glob.bzl", "subdir_glob")

        cxx_library(
            name = "mysql-cpp",
            compiler_flags = [
                '-std=c++2a',
            ],
            header_namespace = "{self._conanfile.header_namespace}",
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
        """)
        for require, dependency in self._conanfile.dependencies.items():
            includeDirectories = []
            staticLibraries = []
            # pprint(dependency.package_folder)
            if (len(dependency.cpp_info.components) > 0):
                for name, component in dependency.cpp_info.components.items():
                    for includeDirectory in component._includedirs:
                        includeDirectories.append(includeDirectory.split(dependency.package_folder, 1)[1][1:])
                    for staticLibrary in component._libs:
                        staticLibraries.append(staticLibrary)

            else:
                if (len(dependency.cpp_info.includedirs) > 0):
                    for includeDirectory in dependency.cpp_info.includedirs:
                        includeDirectories.append(includeDirectory.split(dependency.package_folder, 1)[1][1:])
                for lib in dependency.cpp_info.libs:
                    staticLibraries.append(lib)


            # Generate BUCK file for dependency
            dep_buck_file = ""

            # Generate include directories as a library
            dep_buck_file += dedent(f"""\
            cxx_library(
                name = "headers",
                public_include_directories = [
                """)

            # list(set(...)) conversion to uniqueify
            for includeDirectory in list(set(includeDirectories)):
                dep_buck_file += f"{INDENT * 2}\"{includeDirectory}\",\n"
            dep_buck_file += dedent("""\
                ]
            )
            """)

            # Generate static libraries
            for staticLibrary in staticLibraries:
                dep_buck_file += dedent(f"""
                prebuilt_cxx_library(
                    name = "{staticLibrary}",
                    static_lib = "lib/lib{staticLibrary}.a",
                    visibility = ["PUBLIC"],
                )
                """)

            # Generate static library group
            dep_buck_file += dedent(f"""
            prebuilt_cxx_library_group(
                name = "{dependency.ref.name}",
                exported_deps = [
                    ":headers",
            """)
            for staticLibrary in staticLibraries:
                dep_buck_file += f"{INDENT * 2}\":{staticLibrary}\",\n"
            dep_buck_file += dedent("""\
                ],
                visibility = ["PUBLIC"],
            )
            """)

            regex = "^{source_folder}/?".format(source_folder=self._conanfile.source_folder)
            dep_folder = re.sub(
                regex, r"", dependency.package_folder
            )
            save(self._conanfile, dep_folder + "/BUCK", dep_buck_file)
            buck_file += f"{INDENT * 2}\"//{dep_folder}:{dependency.ref.name}\",\n"

        buck_file += dedent("""\
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
        """)

        # Save the nicely formatted file contents
        save(self._conanfile, "BUCK", buck_file)

class PyReq(ConanFile):
    name = "buck_generator"
    version = "0.1"
    package_type = "python-require"
