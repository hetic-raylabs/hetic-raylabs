#pragma once

#include "materials/Material.hpp"
#include "math/Color.hpp"
#include "math/Vec3.hpp"
#include <cmath>

class Lambertian : public Material {
public:
	Color albedo;

	Lambertian(const Color& a) : albedo(a) {}

	bool scatter(const Ray& ray_in, const HitRecord& rec, Color& attenuation, Ray& scattered) const override {
		Vec3 scatter_direction = rec.normal + random_unit_vector();
		
		if (scatter_direction.length_squared() < 1e-8f) {
			scatter_direction = rec.normal;
		}

		scatter_direction = normalize(scatter_direction);
		Vec3 offset_origin = rec.point + 0.001f * rec.normal;
		scattered = Ray(offset_origin, scatter_direction);
		attenuation = albedo;
		return true;
	}

private:
	static Vec3 random_unit_vector() {
		while (true) {
			Vec3 p = Vec3(random_float(-1, 1), random_float(-1, 1), random_float(-1, 1));
			if (p.length_squared() >= 1.0f) continue;
			return normalize(p);
		}
	}

	static float random_float(float min, float max) {
		static thread_local unsigned int seed = 12345;
		seed = seed * 1103515245 + 12345;
		seed = (seed << 13) ^ seed;
		seed = (seed * (seed * seed * 15731 + 789221) + 1376312589);
		float normalized = (seed & 0x7FFFFFFF) / 2147483647.0f;
		return min + normalized * (max - min);
	}
};

