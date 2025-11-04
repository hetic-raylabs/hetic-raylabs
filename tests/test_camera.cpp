#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include "core/Camera.hpp"

TEST_CASE("Camera default constructor") {
    Camera cam;

    CHECK(cam.position.x == 0.0f);
    CHECK(cam.position.y == 0.0f);
    CHECK(cam.position.z == 0.0f);
    CHECK(cam.fov == 90.0f);
    CHECK(cam.aspect_ratio == doctest::Approx(16.0f / 9.0f));
}

TEST_CASE("Camera with parameters") {
    Point3 pos(1, 2, 3);
    Point3 target(0, 0, 0);
    Vec3 up(0, 1, 0);
    float fov = 45.0f;
    float aspect = 1.5f;

    Camera cam(pos, target, up, fov, aspect);

    CHECK(cam.position.x == 1.0f);
    CHECK(cam.position.y == 2.0f);
    CHECK(cam.position.z == 3.0f);
    CHECK(cam.look_at.x == 0.0f);
    CHECK(cam.fov == 45.0f);
    CHECK(cam.aspect_ratio == 1.5f);
}
