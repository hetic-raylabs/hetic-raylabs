#pragma once

#include <cmath>
#include "core/HitRecord.hpp"
#include "core/Ray.hpp"
#include "entities/Shape.hpp"
#include "math/Vec3.hpp"

class Triangle : public Shape {
   public:
    Point3 a;
    Point3 b;
    Point3 c;

    Triangle() : a(0, 0, 0), b(1, 0, 0), c(0, 1, 0) {}
    Triangle(const Point3& a_, const Point3& b_, const Point3& c_) : a(a_), b(b_), c(c_) {}

    bool hit(const Ray& ray, float tMin, float tMax, HitRecord& hitRecord) const override {
        const float EPS = 1e-6f;
        Vec3 edge1 = b - a;
        Vec3 edge2 = c - a;
        Vec3 pvec = cross(ray.direction, edge2);
        float det = dot(edge1, pvec);
        if (std::fabs(det) < EPS)
            return false;
        float invDet = 1.0f / det;

        Vec3 tvec = ray.origin - a;
        float u = dot(tvec, pvec) * invDet;
        if (u < 0.0f || u > 1.0f)
            return false;

        Vec3 qvec = cross(tvec, edge1);
        float v = dot(ray.direction, qvec) * invDet;
        if (v < 0.0f || u + v > 1.0f)
            return false;

        float t = dot(edge2, qvec) * invDet;
        if (t < tMin || t > tMax)
            return false;

        hitRecord.t = t;
        hitRecord.point = ray.at(t);
        Vec3 outward_normal = normalize(cross(edge1, edge2));
        hitRecord.set_face_normal(ray, outward_normal);
        return true;
    }
};
