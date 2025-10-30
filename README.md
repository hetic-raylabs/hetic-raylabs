## 🧠 Build Commands

### Linux (Docker)

```shell
conan install . -of=build.docker/dev/release --build=missing -s build_type=Release
cmake --preset docker-dev-release
cmake --build --preset docker-dev-release -j
```

### macOS (Apple Silicon)

```shell
conan install . -of=build/macos-arm64-appleclang/debug --build=missing
cmake --preset host-macos-arm64-appleclang-debug
cmake --build --preset host-macos-arm64-appleclang-debug -j
./build/macos-arm64-appleclang/debug/bin/raytracer --scene ./assets/scenes/sample.json --out ./output/out.png
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
