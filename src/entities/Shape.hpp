#pragma once

#include "core/Ray.hpp"
#include "core/HitRecord.hpp"

class Shape {
public:
    virtual ~Shape() = default;

    virtual bool hit(const Ray& ray, float tMin, float tMax, HitRecord& hitRecord) const = 0;
};
