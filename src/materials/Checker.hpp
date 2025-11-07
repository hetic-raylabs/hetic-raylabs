#pragma once

#include <cmath>
#include "materials/Material.hpp"
#include "math/Color.hpp"
#include "math/Vec3.hpp"

class Checker : public Material {
   public:
    Color color1;
    Color color2;
    float scale;

    Checker(const Color& c1, const Color& c2, float s = 1.0f) : color1(c1), color2(c2), scale(s) {}

    bool scatter(const Ray& ray_in, const HitRecord& rec, Color& attenuation,
                 Ray& scattered) const override {
        int xi = static_cast<int>(floorf(rec.point.x * scale));
        int zi = static_cast<int>(floorf(rec.point.z * scale));
        bool checker = ((xi + zi) & 1) == 0;

        Color albedo = checker ? color1 : color2;

        Vec3 unit_direction = normalize(ray_in.direction);
        Vec3 reflected = reflect(unit_direction, rec.normal);
        Vec3 scattered_direction = normalize(reflected);

        Vec3 offset_origin = rec.point + 0.001f * rec.normal;
        scattered = Ray(offset_origin, scattered_direction);
        attenuation = albedo;
        return true;
    }

   private:
    static Vec3 reflect(const Vec3& v, const Vec3& n) { return v - 2.0f * dot(v, n) * n; }
};
