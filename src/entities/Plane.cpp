#include "Plane.hpp"
#include <cmath>

Plane::Plane(const Point3& p, const Vec3& n) : point(p), normal(normalize(n)) {}

bool Plane::hit(const Ray& ray, float tMin, float tMax, HitRecord& hitRecord) const {
    const float EPS = 1e-6f;
    float denom = dot(normal, ray.direction);
    if (std::fabs(denom) < EPS) {
        return false;
    }
    float t = dot(point - ray.origin, normal) / denom;
    if (t < tMin || t > tMax) {
        return false;
    }
    hitRecord.t = t;
    hitRecord.point = ray.at(t);
    hitRecord.set_face_normal(ray, normal);
    return true;
}
