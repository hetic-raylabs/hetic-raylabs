#include <doctest/doctest.h>

#include <memory>

#include "core/Ray.hpp"
#include "core/HitRecord.hpp"
#include "core/Scene.hpp"
#include "entities/Plane.hpp"
#include "entities/Triangle.hpp"

TEST_CASE("Scene hit returns closest shape") {
    Scene scene;
    auto plane = std::make_shared<Plane>(Point3(0,0,0), Vec3(0,1,0));
    auto tri = std::make_shared<Triangle>(Point3(0,0,5), Point3(1,0,5), Point3(0,1,5));
    scene.add(plane);
    scene.add(tri);

    Ray ray(Point3(0, 1, 0), Vec3(0, -1, 0));
    HitRecord rec{};
    bool hit = scene.hit(ray, 0.001f, 1e9f, rec);

    CHECK(hit);
    CHECK(rec.t == doctest::Approx(1.0f));
    CHECK(rec.point.y == doctest::Approx(0.0f));
}
