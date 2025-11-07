#include <doctest/doctest.h>

#include <string>

#include "core/Camera.hpp"
#include "core/Scene.hpp"
#include "io/JsonSceneLoader.hpp"

TEST_CASE("JsonSceneLoader loads camera and objects") {
    std::string jsonText = R"JSON(
{
  "camera": {
    "position": [0, 1, 3],
    "look_at": [0, 0, 0],
    "up": [0, 1, 0],
    "fov": 60.0,
    "aspect_ratio": 1.77778
  },
  "objects": [
    { "type": "plane", "point": [0,0,0], "normal": [0,1,0] }
  ]
}
)JSON";

    Scene scene;
    Camera cam;
    std::string err;
    bool ok = io::JsonSceneLoader::loadFromString(jsonText, scene, cam, &err);
    CHECK_MESSAGE(ok, err.c_str());
    CHECK(scene.entities.size() == 1);
    CHECK(cam.fov == doctest::Approx(60.0f));
    CHECK(cam.aspect_ratio == doctest::Approx(1.77778f));
}
