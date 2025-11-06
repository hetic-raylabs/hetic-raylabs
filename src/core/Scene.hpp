#pragma once

#include <memory>
#include <vector>

#include "core/Ray.hpp"
#include "core/HitRecord.hpp"
#include "entities/Shape.hpp"

class Scene {
public:
	std::vector<std::shared_ptr<Shape>> shapes;

	void add(const std::shared_ptr<Shape>& shape) { shapes.push_back(shape); }

	bool hit(const Ray& ray, float tMin, float tMax, HitRecord& outRecord) const {
		HitRecord temp{};
		bool hitAnything = false;
		float closest = tMax;
		for (const auto& s : shapes) {
			if (s->hit(ray, tMin, closest, temp)) {
				hitAnything = true;
				closest = temp.t;
				outRecord = temp;
			}
		}
		return hitAnything;
	}
};
