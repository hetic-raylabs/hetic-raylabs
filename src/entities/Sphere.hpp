#pragma once

#include "core/HitRecord.hpp"
#include "core/Ray.hpp"
#include "math/Vec3.hpp"

class Sphere {
   public:
    Point3 center;
    float radius;

    Sphere() : center(0, 0, 0), radius(1.0f) {}
    Sphere(const Point3& c, float r) : center(c), radius(r) {}

    // Test if a ray hits this sphere and fill in the hit record
    bool hit(const Ray& ray, float t_min, float t_max, HitRecord& rec) const;
};
