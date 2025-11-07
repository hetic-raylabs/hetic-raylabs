#pragma once

#include "core/HitRecord.hpp"
#include "core/Ray.hpp"

class Shape {
   public:
    virtual ~Shape() = default;

    virtual bool hit(const Ray& ray, float tMin, float tMax, HitRecord& hitRecord) const = 0;
};
