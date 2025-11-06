#pragma once

#include "materials/Material.hpp"
#include "math/Color.hpp"
#include "math/Vec3.hpp"
#include <cmath>

class Metal : public Material {
public:
	Color albedo;
	float fuzz;

	Metal(const Color& a, float f = 0.0f) : albedo(a), fuzz(f < 1.0f ? f : 1.0f) {}

	bool scatter(const Ray& ray_in, const HitRecord& rec, Color& attenuation, Ray& scattered) const override {
		Vec3 reflected = reflect(normalize(ray_in.direction), rec.normal);
		scattered = Ray(rec.point, normalize(reflected + fuzz * random_in_unit_sphere()));
		attenuation = albedo;
		return dot(scattered.direction, rec.normal) > 0;
	}

private:
	static Vec3 reflect(const Vec3& v, const Vec3& n) {
		return v - 2.0f * dot(v, n) * n;
	}

	static Vec3 random_in_unit_sphere() {
		while (true) {
			Vec3 p = Vec3(random_float(-1, 1), random_float(-1, 1), random_float(-1, 1));
			if (p.length_squared() >= 1.0f) continue;
			return p;
		}
	}

	static float random_float(float min, float max) {
		static unsigned int seed = 54321;
		seed = seed * 1103515245 + 12345;
		float normalized = (seed & 0x7FFFFFFF) / 2147483647.0f;
		return min + normalized * (max - min);
	}
};

