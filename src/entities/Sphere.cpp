#include "Sphere.hpp"
#include "math/Math_utils.hpp"

bool Sphere::hit(const Ray& ray, float tMin, float tMax, HitRecord& rec) const {
    Vec3 oc = ray.origin - center;

    float a = dot(ray.direction, ray.direction);
    float half_b = dot(oc, ray.direction);
    float c = dot(oc, oc) - radius * radius;

    float discriminant = half_b * half_b - a * c;

    if (discriminant < 0) {
        return false;
    }

    float sqrtd = raylabs::sqrt(discriminant);

    float root = (-half_b - sqrtd) / a;
    if (root < tMin || root > tMax) {
        root = (-half_b + sqrtd) / a;
        if (root < tMin || root > tMax) {
            return false;
        }
    }

    rec.t = root;
    rec.point = ray.at(rec.t);
    Vec3 outward_normal = (rec.point - center) / radius;
    rec.set_face_normal(ray, outward_normal);

    return true;
}
