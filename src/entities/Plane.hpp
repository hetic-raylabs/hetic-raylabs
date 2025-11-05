#pragma once

#include "core/Ray.hpp"
#include "core/HitRecord.hpp"
#include "math/Vec3.hpp"
#include "entities/Shape.hpp"
#include <cmath>

class Plane : public Shape {
public:
	Point3 point;
	Vec3 normal;

	Plane() : point(0, 0, 0), normal(0, 1, 0) {}
	Plane(const Point3& p, const Vec3& n) : point(p), normal(normalize(n)) {}

	bool hit(const Ray& ray, float tMin, float tMax, HitRecord& hitRecord) const override {
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
};
