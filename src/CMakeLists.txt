# ============================================================
# CMake configuration for the RayLabs project (source folder)
# ============================================================
cmake_minimum_required(VERSION 3.20)

# Create a reusable library for the core engine
add_library(raylabs_lib STATIC)

# ------------------------------------------------------------
# Collect sources
# ------------------------------------------------------------
file(GLOB_RECURSE RAYLABS_CORE_SOURCES
    CONFIGURE_DEPENDS
    core/*.cpp core/*.hpp
    entities/*.cpp entities/*.hpp
    image/*.cpp image/*.hpp
    io/*.cpp io/*.hpp
    lights/*.cpp lights/*.hpp
    lodepng/*.cpp lodepng/*.hpp
    materials/*.cpp materials/*.hpp
    math/*.cpp math/*.hpp
    renderer/*.cpp renderer/*.hpp
    utils/*.cpp utils/*.hpp
    app/CliOptions.cpp app/CliOptions.hpp
)
target_sources(raylabs_lib PRIVATE ${RAYLABS_CORE_SOURCES})

# ------------------------------------------------------------
# Include directories
# ------------------------------------------------------------
target_include_directories(raylabs_lib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# ------------------------------------------------------------
# C++ standard and warnings
# ------------------------------------------------------------
target_compile_features(raylabs_lib PUBLIC cxx_std_20)
if(MSVC)
  target_compile_options(raylabs_lib PRIVATE /W4 /permissive-)
else()
  target_compile_options(raylabs_lib PRIVATE -Wall -Wextra -Werror -pedantic)
endif()

# ------------------------------------------------------------
# Dependencies (consume headers during this target's compilation)
# ------------------------------------------------------------
if (HAVE_NLOHMANN_JSON)
  target_link_libraries(raylabs_lib PRIVATE nlohmann_json::nlohmann_json)
  # Primary: import interface include dirs from the target
  target_include_directories(raylabs_lib PRIVATE
    $<TARGET_PROPERTY:nlohmann_json::nlohmann_json,INTERFACE_INCLUDE_DIRECTORIES>)
  # Fallback: some generators also expose *_INCLUDE_DIRS variables
  if (DEFINED nlohmann_json_INCLUDE_DIRS)
    target_include_directories(raylabs_lib PRIVATE ${nlohmann_json_INCLUDE_DIRS})
  endif()
else()
  message(FATAL_ERROR "nlohmann_json not found. Did you run Conan and pass the generated toolchain/deps?")
endif()

if (HAVE_STB)
  target_link_libraries(raylabs_lib PUBLIC stb::stb)
  target_include_directories(raylabs_lib PRIVATE
    $<TARGET_PROPERTY:stb::stb,INTERFACE_INCLUDE_DIRECTORIES>)
  if (DEFINED stb_INCLUDE_DIRS)
    target_include_directories(raylabs_lib PRIVATE ${stb_INCLUDE_DIRS})
  endif()
endif()

# ------------------------------------------------------------
# Build the main CLI executable
# ------------------------------------------------------------
add_executable(raylabs_app
    app/main.cpp
)

target_include_directories(raylabs_app
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/app
)

target_link_libraries(raylabs_app
    PRIVATE raylabs_lib
)

set_target_properties(raylabs_app PROPERTIES
    OUTPUT_NAME "raylabs"
)

# ------------------------------------------------------------
# Installation targets
# ------------------------------------------------------------
install(TARGETS raylabs_lib raylabs_app
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
