from conan import ConanFile
from conan.tools.cmake import cmake_layout


class RaytracerDeps(ConanFile):
    name = "raytracer-deps"
    version = "0"
    settings = "os", "arch", "compiler", "build_type"
    # Generate toolchain + find_package() files
    generators = ("CMakeToolchain", "CMakeDeps")

    # Declare deps here (same as your previous conanfile.txt)
    requires = (
        "fmt/12.0.0",
        "nlohmann_json/3.12.0",
        "stb/cci.20240531",
        "doctest/2.4.12",
    )

    def layout(self):
        # Put *everything* inside the output-folder root (passed with -of)
        # No Debug/Release subfolders; we’ll pass build_type via -s build_type=...
        cmake_layout(self, build_folder="cmake")
        # Drop generators (toolchain, deps) at the output-folder root
        self.folders.generators = "cmake"
