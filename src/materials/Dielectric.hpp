#pragma once

#include <cmath>
#include "materials/Material.hpp"
#include "math/Color.hpp"
#include "math/Vec3.hpp"

class Dielectric : public Material {
   public:
    float ior;

    Dielectric(float index_of_refraction) : ior(index_of_refraction) {}

    bool scatter(const Ray& ray_in, const HitRecord& rec, Color& attenuation,
                 Ray& scattered) const override {
        attenuation = Color(1.0f, 1.0f, 1.0f);

        float refraction_ratio = rec.front_face ? (1.0f / ior) : ior;

        Vec3 unit_direction = normalize(ray_in.direction);
        float cos_theta = fminf(dot(-unit_direction, rec.normal), 1.0f);
        float sin_theta = sqrtf(1.0f - cos_theta * cos_theta);

        bool cannot_refract = refraction_ratio * sin_theta > 1.0f;
        Vec3 direction;

        if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_float()) {
            direction = reflect(unit_direction, rec.normal);
        } else {
            direction = refract(unit_direction, rec.normal, refraction_ratio);
        }

        Vec3 offset_origin = rec.point + 0.001f * rec.normal;
        scattered = Ray(offset_origin, normalize(direction));
        return true;
    }

   private:
    static Vec3 reflect(const Vec3& v, const Vec3& n) { return v - 2.0f * dot(v, n) * n; }

    static Vec3 refract(const Vec3& uv, const Vec3& n, float etai_over_etat) {
        float cos_theta = fminf(dot(-uv, n), 1.0f);
        Vec3 r_out_perp = etai_over_etat * (uv + cos_theta * n);
        Vec3 r_out_parallel = -sqrtf(fabsf(1.0f - r_out_perp.length_squared())) * n;
        return r_out_perp + r_out_parallel;
    }

    static float reflectance(float cosine, float ref_idx) {
        float r0 = (1.0f - ref_idx) / (1.0f + ref_idx);
        r0 = r0 * r0;
        return r0 + (1.0f - r0) * powf((1.0f - cosine), 5.0f);
    }

    static float random_float() {
        static thread_local unsigned int seed = 11111;
        seed = seed * 1103515245 + 12345;
        seed = (seed << 13) ^ seed;
        seed = (seed * (seed * seed * 15731 + 789221) + 1376312589);
        return (seed & 0x7FFFFFFF) / 2147483647.0f;
    }
};
