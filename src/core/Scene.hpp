#pragma once

#include <memory>
#include <vector>

#include "core/Ray.hpp"
#include "core/HitRecord.hpp"
#include "entities/Shape.hpp"

class Material;

class Scene {
public:
	struct Entity {
		std::shared_ptr<Shape> shape;
		std::shared_ptr<Material> material;
	};

	std::vector<Entity> entities;

	void add(const std::shared_ptr<Shape>& shape, const std::shared_ptr<Material>& material) {
		entities.push_back({shape, material});
	}

	void add(const std::shared_ptr<Shape>& shape) {
		entities.push_back({shape, nullptr});
	}

	bool hit(const Ray& ray, float tMin, float tMax, HitRecord& outRecord) const {
		HitRecord temp{};
		bool hitAnything = false;
		float closest = tMax;
		for (const auto& e : entities) {
			if (e.shape->hit(ray, tMin, closest, temp)) {
				hitAnything = true;
				closest = temp.t;
				temp.material = e.material.get();
				outRecord = temp;
			}
		}
		return hitAnything;
	}
};
