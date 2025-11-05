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

## Docker

```shell
# Down containers + remove volumes (conan2 cache)
docker compose down -v

# Connect to container terminal
docker compose run --rm cpp-dev
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

## 🔨 Tests

```shell
# 1) Install conan packages
conan install . -of=build.docker/dev/debug --build=missing -s build_type=Debug

# 2) Reconfigure (tests activated)
cmake --preset docker-dev-debug -DBUILD_TESTING=ON

# 3) Build
cmake --build --preset docker-dev-debug

# 4) Execute tests (with CTest)
ctest --preset docker-dev-debug --output-on-failure
```

Utilisation rapide

Activer ASan sur les tests :

```shell
cmake --preset docker-dev-debug -DBUILD_TESTING=ON -DRAYLABS_TEST_ASAN=ON
cmake --build --preset docker-dev-debug
ctest --preset docker-dev-debug --output-on-failure
```

Activer Coverage :

```shell
cmake --preset docker-dev-debug -DBUILD_TESTING=ON -DRAYLABS_ENABLE_COVERAGE=ON
cmake --build --preset docker-dev-debug
# puis génère le rapport avec lcov/gcovr/llvm-cov selon ton toolchain
```
