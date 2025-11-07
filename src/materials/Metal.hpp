#pragma once

#include <cmath>
#include "materials/Material.hpp"
#include "math/Color.hpp"
#include "math/Vec3.hpp"

class Metal : public Material {
   public:
    Color albedo;
    float fuzz;

    Metal(const Color& a, float f = 0.0f) : albedo(a), fuzz(f < 1.0f ? f : 1.0f) {}

    bool scatter(const Ray& ray_in, const HitRecord& rec, Color& attenuation,
                 Ray& scattered) const override {
        Vec3 unit_direction = normalize(ray_in.direction);
        Vec3 reflected = reflect(unit_direction, rec.normal);
        Vec3 scattered_direction = reflected + fuzz * random_in_unit_sphere();

        if (scattered_direction.length_squared() < 1e-8f) {
            scattered_direction = reflected;
        }

        scattered_direction = normalize(scattered_direction);

        if (dot(scattered_direction, rec.normal) <= 0.0f) {
            return false;
        }

        Vec3 offset_origin = rec.point + 0.001f * rec.normal;
        scattered = Ray(offset_origin, scattered_direction);
        attenuation = albedo;
        return true;
    }

   private:
    static Vec3 reflect(const Vec3& v, const Vec3& n) { return v - 2.0f * dot(v, n) * n; }

    static Vec3 random_in_unit_sphere() {
        while (true) {
            Vec3 p = Vec3(random_float(-1, 1), random_float(-1, 1), random_float(-1, 1));
            if (p.length_squared() >= 1.0f)
                continue;
            return p;
        }
    }

    static float random_float(float min, float max) {
        static thread_local unsigned int seed = 54321;
        seed = seed * 1103515245 + 12345;
        seed = (seed << 13) ^ seed;
        seed = (seed * (seed * seed * 15731 + 789221) + 1376312589);
        float normalized = (seed & 0x7FFFFFFF) / 2147483647.0f;
        return min + normalized * (max - min);
    }
};
