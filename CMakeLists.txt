cmake_minimum_required(VERSION 3.25)
project(raytracer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Ensure compile_commands.json is written
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Compute the "build profile root" (one level above cmake/)
get_filename_component(BUILD_ROOT_DIR "${CMAKE_BINARY_DIR}/.." ABSOLUTE)

# Global output directories (single-config generators)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${BUILD_ROOT_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${BUILD_ROOT_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${BUILD_ROOT_DIR}/lib")

# Multi-config generators (VS/Xcode): repeat for each config
foreach(cfg IN ITEMS Debug Release RelWithDebInfo MinSizeRel)
  string(TOUPPER "${cfg}" CFGU)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CFGU} "${BUILD_ROOT_DIR}/bin")
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CFGU} "${BUILD_ROOT_DIR}/lib")
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CFGU} "${BUILD_ROOT_DIR}/lib")
endforeach()

# Optional: symlink compile_commands.json to the build root for tool/IDE pickup
execute_process(
  COMMAND ${CMAKE_COMMAND} -E create_symlink
          "${CMAKE_BINARY_DIR}/compile_commands.json"
          "${BUILD_ROOT_DIR}/compile_commands.json"
  RESULT_VARIABLE _SYMLINK_RES
)

# Integrate clang-tidy if available
find_program(CLANG_TIDY_EXE NAMES clang-tidy)
if (CLANG_TIDY_EXE)
  message(STATUS "clang-tidy found: ${CLANG_TIDY_EXE}")
  set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_EXE};-p=${CMAKE_BINARY_DIR}")
endif()

# Test target output directory function
function(rt_mark_test_target tgt)
  get_filename_component(BUILD_ROOT_DIR "${CMAKE_BINARY_DIR}/.." ABSOLUTE)
  set_target_properties(${tgt} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${BUILD_ROOT_DIR}/tests/bin"
  )
  foreach(cfg IN ITEMS Debug Release RelWithDebInfo MinSizeRel)
    string(TOUPPER "${cfg}" CFGU)
    set_target_properties(${tgt} PROPERTIES
      RUNTIME_OUTPUT_DIRECTORY_${CFGU} "${BUILD_ROOT_DIR}/tests/bin"
    )
  endforeach()
endfunction()

# Example:
# add_executable(unit_math tests/unit_math.cpp)
# target_link_libraries(unit_math PRIVATE rt_core doctest::doctest)
# rt_mark_test_target(unit_math)


# Prevent in-source build
if (CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
  message(FATAL_ERROR "Please use an out-of-source build (e.g. ./build/).")
endif()

find_package(fmt CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(stb CONFIG REQUIRED)
find_package(doctest CONFIG QUIET)

# add_subdirectory(src)
# add_subdirectory(tests)

# Source files
add_executable(raytracer src/app/main.cpp)
