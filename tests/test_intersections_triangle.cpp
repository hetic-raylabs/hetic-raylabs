#include <doctest/doctest.h>

#include "core/Ray.hpp"
#include "core/HitRecord.hpp"
#include "math/Vec3.hpp"
#include "entities/Shape.hpp"
#include "entities/Triangle.hpp"

TEST_CASE("Triangle hit - front face") {
    Triangle tri(Point3(0,0,0), Point3(1,0,0), Point3(0,1,0));
    Ray ray(Point3(0.25f, 0.25f, 1.0f), Vec3(0,0,-1));

    HitRecord rec{};
    bool hit = tri.hit(ray, 0.001f, 1e9f, rec);
    CHECK(hit);
    CHECK(rec.front_face == true);
    CHECK(rec.point.x == doctest::Approx(0.25f));
    CHECK(rec.point.y == doctest::Approx(0.25f));
    CHECK(rec.point.z == doctest::Approx(0.0f));
}

TEST_CASE("Triangle miss - outside barycentric") {
    Triangle tri(Point3(0,0,0), Point3(1,0,0), Point3(0,1,0));
    Ray ray(Point3(1.1f, 1.1f, 1.0f), Vec3(0,0,-1));

    HitRecord rec{};
    bool hit = tri.hit(ray, 0.001f, 1e9f, rec);
    CHECK_FALSE(hit);
}

TEST_CASE("Triangle miss - parallel ray") {
    Triangle tri(Point3(0,0,0), Point3(1,0,0), Point3(0,1,0));
    Ray ray(Point3(0.25f, 0.25f, 1.0f), Vec3(1,0,0));

    HitRecord rec{};
    bool hit = tri.hit(ray, 0.001f, 1e9f, rec);
    CHECK_FALSE(hit);
}
