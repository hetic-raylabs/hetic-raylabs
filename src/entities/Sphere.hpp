#pragma once

#include "core/HitRecord.hpp"
#include "core/Ray.hpp"
#include "entities/Shape.hpp"
#include "math/Vec3.hpp"

class Sphere : public Shape {
   public:
    Point3 center;
    float radius;

    Sphere() : center(0, 0, 0), radius(1.0f) {}
    Sphere(const Point3& c, float r) : center(c), radius(r) {}

    bool hit(const Ray& ray, float tMin, float tMax, HitRecord& rec) const override;
};
