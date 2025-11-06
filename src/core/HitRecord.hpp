#pragma once

#include "core/Ray.hpp"
#include "math/Vec3.hpp"

class Material;

struct HitRecord {
    Point3 point;
    Vec3 normal;
    float t;
    bool front_face;
    const Material* material = nullptr;

    void set_face_normal(const Ray& ray, const Vec3& outward_normal) {
        front_face = dot(ray.direction, outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};
