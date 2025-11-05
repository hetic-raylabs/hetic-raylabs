#include <doctest/doctest.h>
#include "core/HitRecord.hpp"
#include "core/Ray.hpp"

TEST_CASE("Ray construction") {
    Point3 origin(1, 2, 3);
    Vec3 direction(0, 0, 1);

    Ray ray(origin, direction);

    CHECK(ray.origin.x == 1.0f);
    CHECK(ray.origin.y == 2.0f);
    CHECK(ray.origin.z == 3.0f);
    CHECK(ray.direction.x == 0.0f);
    CHECK(ray.direction.y == 0.0f);
    CHECK(ray.direction.z == 1.0f);
}

TEST_CASE("Ray default constructor") {
    Ray ray;
    CHECK(ray.origin.x == 0.0f);
    CHECK(ray.origin.y == 0.0f);
    CHECK(ray.origin.z == 0.0f);
}

TEST_CASE("Ray at() function") {
    Ray ray(Point3(0, 0, 0), Vec3(1, 0, 0));

    Point3 p0 = ray.at(0.0f);
    CHECK(p0.x == 0.0f);
    CHECK(p0.y == 0.0f);
    CHECK(p0.z == 0.0f);

    Point3 p5 = ray.at(5.0f);
    CHECK(p5.x == 5.0f);
    CHECK(p5.y == 0.0f);
    CHECK(p5.z == 0.0f);

    Point3 p_neg = ray.at(-2.0f);
    CHECK(p_neg.x == -2.0f);
}

TEST_CASE("Ray at() with direction vector") {
    Ray ray(Point3(1, 2, 3), Vec3(1, 1, 1));

    Point3 p = ray.at(2.0f);
    CHECK(p.x == 3.0f);
    CHECK(p.y == 4.0f);
    CHECK(p.z == 5.0f);
}

TEST_CASE("HitRecord basic usage") {
    HitRecord rec;
    rec.point = Point3(1, 2, 3);
    rec.normal = Vec3(0, 1, 0);
    rec.t = 5.0f;

    CHECK(rec.point.x == 1.0f);
    CHECK(rec.point.y == 2.0f);
    CHECK(rec.point.z == 3.0f);
    CHECK(rec.normal.x == 0.0f);
    CHECK(rec.normal.y == 1.0f);
    CHECK(rec.normal.z == 0.0f);
    CHECK(rec.t == 5.0f);
}

TEST_CASE("HitRecord set_face_normal - front face") {
    Ray ray(Point3(0, 0, 0), Vec3(0, 0, 1));
    Vec3 outward_normal(0, 0, -1);

    HitRecord rec;
    rec.set_face_normal(ray, outward_normal);

    CHECK(rec.front_face == true);
    CHECK(rec.normal.z == -1.0f);
}

TEST_CASE("HitRecord set_face_normal - back face") {
    Ray ray(Point3(0, 0, 0), Vec3(0, 0, 1));
    Vec3 outward_normal(0, 0, 1);

    HitRecord rec;
    rec.set_face_normal(ray, outward_normal);

    CHECK(rec.front_face == false);
    CHECK(rec.normal.z == -1.0f);
}
