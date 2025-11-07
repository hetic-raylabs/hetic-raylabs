#include <doctest/doctest.h>

#include <string>

#include "core/Scene.hpp"
#include "io/JsonSceneLoader.hpp"

TEST_CASE("JsonSceneLoader loads plane and triangle") {
    std::string jsonText = R"JSON(
{
  "objects": [
    { "type": "plane", "point": [0,0,0], "normal": [0,1,0] },
    { "type": "triangle", "a": [0,0,5], "b": [1,0,5], "c": [0,1,5] }
  ]
}
)JSON";

    Scene scene;
    std::string err;
    bool ok = io::JsonSceneLoader::loadFromString(jsonText, scene, &err);
    CHECK_MESSAGE(ok, err.c_str());
    CHECK(scene.entities.size() == 2);
}
