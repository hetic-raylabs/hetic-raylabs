#pragma once

#include "core/Ray.hpp"
#include "core/HitRecord.hpp"
#include "math/Color.hpp"

class Material {
public:
	virtual ~Material() = default;

	virtual bool scatter(const Ray& ray_in, const HitRecord& rec, Color& attenuation, Ray& scattered) const = 0;
};

