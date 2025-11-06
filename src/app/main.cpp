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
#include "materials/Lambertian.hpp"
#include "materials/Metal.hpp"
#include "materials/Checker.hpp"

using namespace std;

static Color sky_color(const Vec3& dir) {
	float t = 0.5f * (normalize(dir).y + 1.0f);
	float r = (1.0f - t) * 1.0f + t * 0.5f;
	float g = (1.0f - t) * 1.0f + t * 0.7f;
	float b = (1.0f - t) * 1.0f + t * 1.0f;
	return Color(r, g, b);
}

static Color ray_color(const Ray& ray, const Scene& scene, int depth) {
	if (depth <= 0) {
		return Color(0.0f, 0.0f, 0.0f);
	}

	HitRecord rec;
	if (scene.hit(ray, 0.001f, 1e9f, rec)) {
		if (rec.material) {
			Ray scattered;
			Color attenuation;
			if (rec.material->scatter(ray, rec, attenuation, scattered)) {
				Color recurse_color = ray_color(scattered, scene, depth - 1);
				return Color(
					attenuation.R() * recurse_color.R(),
					attenuation.G() * recurse_color.G(),
					attenuation.B() * recurse_color.B()
				);
			}
			return Color(0, 0, 0);
		}
		return Color(0.5f, 0.5f, 0.5f);
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
	auto ground_material = std::make_shared<Checker>(Color(0.8f, 0.8f, 0.8f), Color(0.2f, 0.2f, 0.2f), 1.0f);
	auto sphere_material = std::make_shared<Metal>(Color(0.8f, 0.8f, 0.8f), 0.3f);
	scene.add(std::make_shared<Plane>(Point3(0, 0, 0), Vec3(0, 1, 0)), ground_material);
	scene.add(std::make_shared<Sphere>(Point3(0, 0.5f, -2.0f), 0.5f), sphere_material);

	Image image(image_width, image_height);

	cout << "Rendering..." << endl;
	for (int y = 0; y < image_height; y++) {
		for (int x = 0; x < image_width; x++) {
			float u = (x + 0.5f) / float(image_width);
			float v = 1.0f - (y + 0.5f) / float(image_height);
			Ray r = camera.get_ray(u, v);

			Color pixel_color = ray_color(r, scene, 4);
			image.SetPixel(x, y, pixel_color);
		}
	}

	cout << "Writing to file..." << endl;
	image.WriteFile("./output/test.png");

	return 0;
}
