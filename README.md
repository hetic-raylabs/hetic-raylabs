# Raytracer Raylabs

Projet de raytracer en C++ pur développé dans le cadre du cours HETIC.

## 🚀 Installation rapide (Docker - Recommandé)

```bash
# Build + tests
docker-compose run --rm cpp-dev bash -lc "cmake --build --preset docker-dev-release -j && cd build.docker/dev/release/cmake && ctest --output-on-failure -j"

# Générer une image (écrit output/test.png)
docker-compose run --rm cpp-dev bash -lc "cmake --build --preset docker-dev-release -j && ./build.docker/dev/release/bin/raytracer && ls -l output/test.png"
```

## 🧪 Exécuter uniquement les tests

```bash
docker-compose run --rm cpp-dev bash -lc "cd build.docker/dev/release/cmake && ctest --output-on-failure -j"
```

## ⚙️ Build local (exemples)

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
```

### Windows (MSVC)

```shell
conan install . -of=build/windows-x86_64-msvc/debug --build=missing
cmake --preset host-windows-msvc-debug
cmake --build --preset host-windows-msvc-debug -j
```

## ▶️ Run (avec fichier de scène optionnel)

```shell
# Binaire par défaut (génère output/test.png)
./build.docker/dev/release/bin/raytracer

# Exemple si rendu par fichier de scène ajouté plus tard
# ./build/raytracer --scene ./assets/scenes/sample.json --out ./output/out.png
```
