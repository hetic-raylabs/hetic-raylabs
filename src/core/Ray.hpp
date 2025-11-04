#pragma once

#include "math/Vec3.hpp"

class Ray {
   public:
    Point3 origin;
    Vec3 direction;

    Ray() {}
    Ray(const Point3& origin, const Vec3& direction) : origin(origin), direction(direction) {}

    Point3 at(float t) const { return origin + t * direction; }
};

