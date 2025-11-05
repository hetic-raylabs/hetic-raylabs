#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include "core/Ray.hpp"
#include "core/HitRecord.hpp"
#include "math/Vec3.hpp"
#include "entities/Shape.hpp"
#include "entities/Plane.hpp"

TEST_CASE("Plane hit - front face") {
    Plane plane(Point3(0, 0, 0), Vec3(0, 1, 0));
    Ray ray(Point3(0, 1, 0), Vec3(0, -1, 0));

    HitRecord rec{};
    bool hit = plane.hit(ray, 0.001f, 1e9f, rec);
    CHECK(hit);
    CHECK(rec.front_face == true);
    CHECK(rec.normal.x == doctest::Approx(0.0f));
    CHECK(rec.normal.y == doctest::Approx(1.0f));
    CHECK(rec.normal.z == doctest::Approx(0.0f));
    CHECK(rec.point.y == doctest::Approx(0.0f));
    CHECK(rec.t == doctest::Approx(1.0f));
}

TEST_CASE("Plane hit - back face flips normal") {
    Plane plane(Point3(0, 0, 0), Vec3(0, 1, 0));
    Ray ray(Point3(0, -1, 0), Vec3(0, 1, 0));

    HitRecord rec{};
    bool hit = plane.hit(ray, 0.001f, 1e9f, rec);
    CHECK(hit);
    CHECK(rec.front_face == false);
    CHECK(rec.normal.x == doctest::Approx(0.0f));
    CHECK(rec.normal.y == doctest::Approx(-1.0f));
    CHECK(rec.normal.z == doctest::Approx(0.0f));
    CHECK(rec.point.y == doctest::Approx(0.0f));
    CHECK(rec.t == doctest::Approx(1.0f));
}

TEST_CASE("Plane miss - parallel ray") {
    Plane plane(Point3(0, 0, 0), Vec3(0, 1, 0));
    Ray ray(Point3(0, 1, 0), Vec3(1, 0, 0));

    HitRecord rec{};
    bool hit = plane.hit(ray, 0.001f, 1e9f, rec);
    CHECK_FALSE(hit);
}

TEST_CASE("Plane miss - intersection behind origin") {
    Plane plane(Point3(0, 0, 0), Vec3(0, 1, 0));
    Ray ray(Point3(0, -1, 0), Vec3(0, -1, 0));

    HitRecord rec{};
    bool hit = plane.hit(ray, 0.001f, 1e9f, rec);
    CHECK_FALSE(hit);
}
