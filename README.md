# Raytracer Raylabs

Projet de raytracer en C++ pur développé dans le cadre du cours HETIC.

## 🚀 Installation rapide (Docker - Recommandé)

```bash
# Build complet (première installation)
docker-compose run --rm cpp-dev bash -c "conan profile detect --force && conan install . --output-folder=build.docker/dev/release --build=missing --settings=build_type=Release && cmake --preset conan-release && cmake --build --preset conan-release"

# Tests
docker-compose run --rm cpp-dev bash -c "cd build.docker/dev/release/cmake/Release && ctest --output-on-failure"

# Run
docker-compose run --rm cpp-dev bash -c "./build.docker/dev/release/cmake/bin/raytracer"
```

## 🧠 Build Commands

### Linux (Docker)

```shell
conan install . -of=build.docker/dev/release --build=missing -s build_type=Release
cmake --preset conan-release
cmake --build --preset conan-release -j
```

### macOS (Apple Silicon)

```shell
conan install . -of=build/macos-arm64-appleclang/debug --build=missing
cmake --preset host-macos-arm64-appleclang-debug
cmake --build --preset host-macos-arm64-appleclang-debug -j
```

### Windows (MSVC or Clang-cl)

```shell
conan install . -of=build/windows-x86_64-msvc/debug --build=missing
cmake --preset host-windows-msvc-debug
cmake --build --preset host-windows-msvc-debug -j
```

## Run

```shell
./build/raytracer --scene ./assets/scenes/sample.json --out ./output/out.png
```
