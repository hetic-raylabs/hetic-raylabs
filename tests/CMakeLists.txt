# ============================================================
# RayLabs - Unit Tests CMake
# - Auto-discovers test_*.cpp (top-level and subdirs)
# - Skips empty files (size == 0)
# - One executable per test file with doctest main
# - Outputs test binaries to <build_root>/tests/bin
# - Optional toggles: ASan and Coverage
# ============================================================
cmake_minimum_required(VERSION 3.25)

# ------------------------------------------------------------
# Early exit if testing is disabled
# ------------------------------------------------------------
if(NOT BUILD_TESTING)
  message(STATUS "[tests] BUILD_TESTING=OFF -> skipping tests configuration")
  return()
endif()

# ------------------------------------------------------------
# Options
# ------------------------------------------------------------
option(RAYLABS_FETCH_DOCTEST "Fetch doctest if not provided by the system" OFF)
option(RAYLABS_TEST_ASAN "Enable AddressSanitizer for test targets (Clang/GCC)" OFF)
option(RAYLABS_ENABLE_COVERAGE "Enable coverage flags for test targets (Clang/GCC)" OFF)

# ------------------------------------------------------------
# Dependencies (doctest)
# ------------------------------------------------------------
find_package(doctest CONFIG QUIET)
if(NOT doctest_FOUND AND RAYLABS_FETCH_DOCTEST)
  include(FetchContent)
  message(STATUS "[tests] doctest not found; fetching…")
  FetchContent_Declare(
    doctest
    GIT_REPOSITORY https://github.com/doctest/doctest.git
    GIT_TAG v2.4.11
  )
  FetchContent_MakeAvailable(doctest)
  find_package(doctest CONFIG REQUIRED)
elseif(NOT doctest_FOUND)
  message(WARNING "[tests] doctest not found and RAYLABS_FETCH_DOCTEST=OFF -> tests disabled.")
  return()
endif()

# ------------------------------------------------------------
# Helper: set test binaries output directory
# ------------------------------------------------------------
function(raylabs_mark_test_target tgt)
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

# ------------------------------------------------------------
# Helper: apply ASan and Coverage flags to a target (best-effort)
# ------------------------------------------------------------
function(raylabs_apply_test_instrumentation tgt)
  if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    if (RAYLABS_TEST_ASAN)
      # AddressSanitizer for Clang/GCC on Linux/macOS
      target_compile_options(${tgt} PRIVATE -fsanitize=address -fno-omit-frame-pointer)
      target_link_options(${tgt} PRIVATE -fsanitize=address)
    endif()

    if (RAYLABS_ENABLE_COVERAGE)
      # Use --coverage (GCov/LCov/llvm-cov). Keep O0 to improve mapping.
      target_compile_options(${tgt} PRIVATE --coverage -O0)
      target_link_options(${tgt} PRIVATE --coverage)
    endif()
  elseif(MSVC)
    if (RAYLABS_TEST_ASAN)
      message(WARNING "[tests] ASan requested but not auto-configured for MSVC in this template.")
    endif()
    if (RAYLABS_ENABLE_COVERAGE)
      message(WARNING "[tests] Coverage requested but not auto-configured for MSVC in this template.")
    endif()
  endif()
endfunction()

# ------------------------------------------------------------
# Discover test sources (top-level + subdirectories)
# ------------------------------------------------------------
file(GLOB RAYLABS_TEST_SOURCES_TOP
  CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/test_*.cpp"
)
file(GLOB_RECURSE RAYLABS_TEST_SOURCES_RECURSE
  CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/*/test_*.cpp"
)

set(RAYLABS_TEST_SOURCES
  ${RAYLABS_TEST_SOURCES_TOP}
  ${RAYLABS_TEST_SOURCES_RECURSE}
)

# ------------------------------------------------------------
# Filter out empty files (size == 0)
# ------------------------------------------------------------
set(FILTERED_TEST_SOURCES "")
foreach(src ${RAYLABS_TEST_SOURCES})
  if(EXISTS "${src}")
    file(SIZE "${src}" _sz)
    if(_sz GREATER 0)
      list(APPEND FILTERED_TEST_SOURCES "${src}")
    else()
      message(STATUS "[tests] Skipping empty test file: ${src}")
    endif()
  endif()
endforeach()

if(FILTERED_TEST_SOURCES STREQUAL "")
  message(WARNING "[tests] No non-empty test_*.cpp files found under: ${CMAKE_CURRENT_SOURCE_DIR}")
  return()
endif()

# ------------------------------------------------------------
# Build one test executable per source
# ------------------------------------------------------------
enable_testing()

set(RAYLABS_TEST_INCLUDE_DIRS
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_BINARY_DIR}
)

foreach(src ${FILTERED_TEST_SOURCES})
  get_filename_component(tname "${src}" NAME_WE) # e.g. test_foo.cpp -> test_foo

  add_executable(${tname} "${src}")
  target_include_directories(${tname} PRIVATE ${RAYLABS_TEST_INCLUDE_DIRS})
  target_link_libraries(${tname} PRIVATE raylabs_lib doctest::doctest)
  target_compile_definitions(${tname} PRIVATE DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN)

  if(MSVC)
    target_compile_options(${tname} PRIVATE /W4)
  else()
    target_compile_options(${tname} PRIVATE -Wall -Wextra -Wpedantic)
  endif()

  raylabs_mark_test_target(${tname})
  raylabs_apply_test_instrumentation(${tname})

  add_test(NAME ${tname} COMMAND ${tname})
  set_tests_properties(${tname} PROPERTIES LABELS "unit")
endforeach()

# ------------------------------------------------------------
# IDE grouping (optional)
# ------------------------------------------------------------
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
foreach(src ${FILTERED_TEST_SOURCES})
  get_filename_component(tname "${src}" NAME_WE)
  set_property(TARGET ${tname} PROPERTY FOLDER "tests")
endforeach()
