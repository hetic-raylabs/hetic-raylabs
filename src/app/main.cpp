#include <iostream>
#include <memory>
#include "core/Camera.hpp"
#include "core/HitRecord.hpp"
#include "core/Ray.hpp"
#include "core/Scene.hpp"
#include "entities/Plane.hpp"
#include "entities/Sphere.hpp"
#include "image/Image.hpp"
#include "math/Color.hpp"
#include "math/Vec3.hpp"

using namespace std;

static Color sky_color(const Vec3& dir) {
	float t = 0.5f * (normalize(dir).y + 1.0f);
	float r = (1.0f - t) * 1.0f + t * 0.5f;
	float g = (1.0f - t) * 1.0f + t * 0.7f;
	float b = (1.0f - t) * 1.0f + t * 1.0f;
	return Color(r, g, b);
}

static Color ground_color(const Point3& p) {
	int xi = static_cast<int>(floorf(p.x));
	int zi = static_cast<int>(floorf(p.z));
	bool checker = ((xi + zi) & 1) == 0;
	return checker ? Color(0.8f, 0.8f, 0.8f) : Color(0.2f, 0.2f, 0.2f);
}

static Color sphere_color(const HitRecord& rec) {
	float gold_r = 1.0f;
	float gold_g = 0.84f;
	float gold_b = 0.0f;

	Vec3 light_direction = normalize(Vec3(1.0f, 1.0f, 1.0f));
	float light_intensity = dot(rec.normal, light_direction);

	if (light_intensity < 0.2f) light_intensity = 0.2f;
	if (light_intensity > 1.0f) light_intensity = 1.0f;

	return Color(gold_r * light_intensity, gold_g * light_intensity, gold_b * light_intensity);
}

static Color ray_color(const Ray& ray, const Scene& scene) {
	HitRecord rec;
	if (scene.hit(ray, 0.001f, 1e9f, rec)) {
		// Identifier si c'est une sphere ou un plane par la position
		// Simple heuristique: si le point est proche du plan y=0, c'est le plane
		if (std::abs(rec.point.y) < 0.01f) {
			return ground_color(rec.point);
		} else {
			return sphere_color(rec);
		}
	}
	return sky_color(ray.direction);
}

int main() {
	const float aspect_ratio = 16.0f / 9.0f;
	const int image_width = 800;
	const int image_height = static_cast<int>(image_width / aspect_ratio);

	cout << "Rendering image of size " << image_width << "x" << image_height << endl;

	Camera camera(Point3(0, 1, 3), Point3(0, 0.3f, 0), Vec3(0, 1, 0), 60.0f, aspect_ratio);

	Scene scene;
	scene.add(std::make_shared<Plane>(Point3(0, 0, 0), Vec3(0, 1, 0)));
	scene.add(std::make_shared<Sphere>(Point3(0, 0.5f, -2.0f), 0.5f));

	Image image(image_width, image_height);

	cout << "Rendering..." << endl;
	for (int y = 0; y < image_height; y++) {
		for (int x = 0; x < image_width; x++) {
			float u = (x + 0.5f) / float(image_width);
			float v = 1.0f - (y + 0.5f) / float(image_height);
			Ray r = camera.get_ray(u, v);

			Color pixel_color = ray_color(r, scene);
			image.SetPixel(x, y, pixel_color);
		}
	}

	cout << "Writing to file..." << endl;
	image.WriteFile("./output/test.png");

	return 0;
}
