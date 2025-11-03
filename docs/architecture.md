## Structure de projet proposée (POO propre)

```
PROJECT_FOLDER/
├─ CMakeLists.txt
├─ CMakePresets.json
├─ conanfile.txt
├─ docker-compose.yml
├─ Dockerfile
├─ .clang-format
├─ .clang-tidy
├─ assets/
│  └─ scenes/
│     └─ sample.json
├─ output/                     # volume de sortie (bind)
├─ build/                      # dossier de build (hors VCS)
├─ src/
│  ├─ app/
│  │  ├─ main.cpp              # parse args, charge JSON, lance le renderer
│  │  └─ CliOptions.hpp/.cpp   # gestion CLI
│  ├─ core/
│  │  ├─ Ray.hpp
│  │  ├─ HitRecord.hpp
│  │  ├─ AABB.hpp/.cpp         # acceleration bounds
│  │  ├─ Camera.hpp/.cpp
│  │  ├─ Sampler.hpp/.cpp      # RNG / stratified sampling
│  │  ├─ Integrator.hpp/.cpp   # interface + PathTracer / Whitted
│  │  ├─ Scene.hpp/.cpp
│  │  └─ BVH.hpp/.cpp          # accélération
│  ├─ entities/
│  │  ├─ Entity.hpp            # base: transform, materialId, name
│  │  ├─ Mesh.hpp/.cpp         # si besoin maillage
│  │  ├─ Plane.hpp/.cpp        # plateau
│  │  ├─ Sphere.hpp/.cpp
│  │  ├─ Box.hpp/.cpp
│  │  └─ Shape.hpp/.cpp        # interface shape: intersect(), bounds()
│  ├─ materials/
│  │  ├─ Material.hpp          # interface: bsdf(), emit()
│  │  ├─ Lambertian.hpp/.cpp
│  │  ├─ Metal.hpp/.cpp
│  │  └─ Dielectric.hpp/.cpp
│  ├─ lights/
│  │  ├─ Light.hpp             # interface: sample(), power()
│  │  ├─ PointLight.hpp/.cpp
│  │  └─ AreaLight.hpp/.cpp
│  ├─ math/
│  │  ├─ Vec3.hpp
│  │  ├─ Color.hpp
│  │  ├─ Transform.hpp/.cpp
│  │  └─ Utils.hpp
│  ├─ io/
│  │  ├─ JsonSceneLoader.hpp/.cpp  # lecture JSON -> Scene/Entities/Materials
│  │  ├─ ImageWriter.hpp/.cpp      # PNG via stb_image_write
│  │  └─ Logger.hpp/.cpp
│  ├─ renderer/
│  │  ├─ Renderer.hpp              # orchestrateur: traverse pixels, appels intégrateur
│  │  ├─ TileScheduler.hpp/.cpp    # parallélisation simple
│  │  └─ FrameBuffer.hpp/.cpp
│  ├─ utils/
│  │  ├─ IdMap.hpp/.cpp            # tables entités <-> ids
│  │  └─ Profiler.hpp/.cpp
│  └─ CMakeLists.txt               # cible "raytracer" + lib interne
├─ tests/
│  ├─ CMakeLists.txt
│  ├─ test_intersections.cpp
│  └─ test_json_loader.cpp
└─ docs/
   └─ architecture.md
```
