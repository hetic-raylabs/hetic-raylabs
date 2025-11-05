# ============================================================
# Root CMake for RayLabs
# - Out-of-source builds only
# - Global output dirs (bin/lib)
# - compile_commands.json
# - Optional clang-tidy
# - Adds src/ (library + CLI) and tests/
# ============================================================
cmake_minimum_required(VERSION 3.25)
project(RayLabs LANGUAGES CXX)

# ------------------------------------------------------------
# C++ standard and compile_commands.json
# ------------------------------------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ------------------------------------------------------------
# Forbid in-source builds
# ------------------------------------------------------------
if (CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
  message(FATAL_ERROR "Please use an out-of-source build, e.g. `cmake -S . -B build`.")
endif()

# ------------------------------------------------------------
# Compute build root (one level above the current binary dir)
# ------------------------------------------------------------
get_filename_component(BUILD_ROOT_DIR "${CMAKE_BINARY_DIR}/.." ABSOLUTE)

# Global output directories (single-config generators)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${BUILD_ROOT_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${BUILD_ROOT_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${BUILD_ROOT_DIR}/lib")

# Multi-config generators (VS/Xcode)
foreach(cfg IN ITEMS Debug Release RelWithDebInfo MinSizeRel)
  string(TOUPPER "${cfg}" CFGU)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CFGU} "${BUILD_ROOT_DIR}/bin")
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CFGU} "${BUILD_ROOT_DIR}/lib")
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CFGU} "${BUILD_ROOT_DIR}/lib")
endforeach()

# ------------------------------------------------------------
# Optional symlink of compile_commands.json at build root
# ------------------------------------------------------------
execute_process(
  COMMAND ${CMAKE_COMMAND} -E create_symlink
          "${CMAKE_BINARY_DIR}/compile_commands.json"
          "${BUILD_ROOT_DIR}/compile_commands.json"
  RESULT_VARIABLE _SYMLINK_RES
)

# ------------------------------------------------------------
# Optional static analysis (clang-tidy)
# ------------------------------------------------------------
option(ENABLE_CLANG_TIDY "Enable clang-tidy if available" OFF)
if (ENABLE_CLANG_TIDY)
  find_program(CLANG_TIDY_EXE NAMES clang-tidy)
  if (CLANG_TIDY_EXE)
    message(STATUS "clang-tidy found: ${CLANG_TIDY_EXE}")
    set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_EXE};-p=${CMAKE_BINARY_DIR}")
  else()
    message(WARNING "ENABLE_CLANG_TIDY=ON but clang-tidy was not found.")
  endif()
endif()

# ------------------------------------------------------------
# Third-party packages (optional, via Conan/vcpkg/system)
# Keep them at root if they are shared by lib/app/tests.
# ------------------------------------------------------------
find_package(nlohmann_json CONFIG QUIET)
find_package(stb CONFIG QUIET)

# Expose availability as cache variables for children
set(HAVE_NLOHMANN_JSON ${nlohmann_json_FOUND} CACHE BOOL "nlohmann_json available")
set(HAVE_STB ${stb_FOUND} CACHE BOOL "stb available")

# ------------------------------------------------------------
# Add source tree (builds raylabs_lib and raylabs_app)
# ------------------------------------------------------------
add_subdirectory(src)

# ------------------------------------------------------------
# Testing (CTest toggled by BUILD_TESTING)
# - tests/CMakeLists.txt creates test targets and registers them
# ------------------------------------------------------------
include(CTest)
if (BUILD_TESTING)
  add_subdirectory(tests)
endif()

# ------------------------------------------------------------
# Install (library + CLI installed from src/)
# Packaging configuration is left to the project-level scripts.
# ------------------------------------------------------------
