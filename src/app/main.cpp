#include <iostream>
#include <memory>
#include <fstream>
#include <sstream>
#include <string>
#include "core/Camera.hpp"
#include "core/Scene.hpp"
#include "core/PathTracer.hpp"
#include "core/Sampler.hpp"
#include "renderer/Renderer.hpp"
#include "io/JsonSceneLoader.hpp"

using namespace std;
using namespace raylabs;

int main(int argc, char* argv[]) {
    string scene_file = "assets/scenes/multiple_spheres.json";
    if (argc > 1) {
        scene_file = argv[1];
    }

    try {
        // Load scene from JSON
        io::SceneDTO scene_dto = io::JsonSceneLoader::load_from_file(scene_file);

        // Populate Scene and Camera from DTO
        Scene scene;
        Camera camera;
        io::JsonSceneLoader::populateScene(scene_dto, scene, camera);

        // Create integrator
        auto integrator = std::make_shared<PathTracer>();

        // Create renderer
        Renderer renderer(scene, camera, scene_dto.image, integrator);

        // Render
        renderer.render();

        return 0;
    } catch (const std::exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
}
