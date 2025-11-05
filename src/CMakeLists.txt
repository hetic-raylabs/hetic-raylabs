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
target_compile_options(raylabs_lib PRIVATE
    -Wall -Wextra -Werror -pedantic
)

# ------------------------------------------------------------
# Dependencies (if any via Conan or FetchContent)
# ------------------------------------------------------------
# Example: link PNG library or JSON
# target_link_libraries(raylabs_lib PUBLIC PNG::PNG nlohmann_json::nlohmann_json)

# ------------------------------------------------------------
# Build the main CLI executable
# ------------------------------------------------------------
add_executable(raylabs_app
    app/main.cpp
    app/CliOptions.cpp
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

# ------------------------------------------------------------
# Optional: testing support
# ------------------------------------------------------------
# if(BUILD_TESTING)
#     add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../tests ${CMAKE_BINARY_DIR}/tests)
# endif()
