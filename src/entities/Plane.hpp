#pragma once

#include "core/HitRecord.hpp"
#include "core/Ray.hpp"
#include "entities/Shape.hpp"
#include "math/Vec3.hpp"

class Plane : public Shape {
   public:
    Point3 point;
    Vec3 normal;

    Plane() : point(0, 0, 0), normal(0, 1, 0) {}
    Plane(const Point3& p, const Vec3& n);

    bool hit(const Ray& ray, float tMin, float tMax, HitRecord& hitRecord) const override;
};
