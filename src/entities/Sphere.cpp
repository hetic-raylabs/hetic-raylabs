#include "Sphere.hpp"
#include "math/math_utils.hpp"

bool Sphere::hit(const Ray& ray, float t_min, float t_max, HitRecord& rec) const {
    // Calcul de l'intersection rayon-sphère
    Vec3 oc = ray.origin - center;

    // Coefficients de l'équation quadratique
    float a = dot(ray.direction, ray.direction);
    float half_b = dot(oc, ray.direction);
    float c = dot(oc, oc) - radius * radius;

    // Discriminant
    float discriminant = half_b * half_b - a * c;

    // Pas d'intersection si le discriminant est négatif
    if (discriminant < 0) {
        return false;
    }

    float sqrtd = raylabs::sqrt(discriminant);

    // Trouver la racine la plus proche dans le range [t_min, t_max]
    float root = (-half_b - sqrtd) / a;
    if (root < t_min || root > t_max) {
        root = (-half_b + sqrtd) / a;
        if (root < t_min || root > t_max) {
            return false;
        }
    }

    // Remplir le hit record avec les informations d'intersection
    rec.t = root;
    rec.point = ray.at(rec.t);
    Vec3 outward_normal = (rec.point - center) / radius;
    rec.set_face_normal(ray, outward_normal);

    return true;
}
