#include <iostream>
#include <memory>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>
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
#include "io/JsonSceneLoader.hpp"

using namespace std;

static float random_float() {
	static thread_local unsigned int seed = 12345;
	seed = seed * 1103515245 + 12345;
	seed = (seed << 16) ^ (seed >> 16);
	seed = seed * 224250251 + 198491317;
	seed = (seed << 13) ^ (seed >> 19);
	return (seed & 0x7FFFFFFF) / 2147483647.0f;
}

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

int main(int argc, char* argv[]) {
	const float aspect_ratio = 16.0f / 9.0f;
	const int image_width = 800;
	const int image_height = static_cast<int>(image_width / aspect_ratio);

	cout << "Rendering image of size " << image_width << "x" << image_height << endl;

	Scene scene;
	Camera camera(Point3(0, 1, 3), Point3(0, 0.3f, 0), Vec3(0, 1, 0), 60.0f, aspect_ratio);

	string scene_file = "assets/scenes/multiple_spheres.json";
	if (argc > 1) {
		scene_file = argv[1];
	}

	string err;
	ifstream file(scene_file);
	if (file) {
		stringstream buffer;
		buffer << file.rdbuf();
		string json_text = buffer.str();
		bool loaded = io::JsonSceneLoader::loadFromString(json_text, scene, camera, &err);
		if (!loaded) {
			cerr << "JSON load failed: " << err << ". Using fallback scene." << endl;
			auto ground_material = std::make_shared<Checker>(Color(0.8f, 0.8f, 0.8f), Color(0.2f, 0.2f, 0.2f), 1.0f);
			auto sphere_material = std::make_shared<Metal>(Color(0.8f, 0.8f, 0.8f), 0.3f);
			scene.add(std::make_shared<Plane>(Point3(0, 0, 0), Vec3(0, 1, 0)), ground_material);
			scene.add(std::make_shared<Sphere>(Point3(0, 0.5f, -2.0f), 0.5f), sphere_material);
		}
	} else {
		cerr << "Failed to open scene file: " << scene_file << ". Using fallback scene." << endl;
		auto ground_material = std::make_shared<Checker>(Color(0.8f, 0.8f, 0.8f), Color(0.2f, 0.2f, 0.2f), 1.0f);
		auto sphere_material = std::make_shared<Metal>(Color(0.8f, 0.8f, 0.8f), 0.3f);
		scene.add(std::make_shared<Plane>(Point3(0, 0, 0), Vec3(0, 1, 0)), ground_material);
		scene.add(std::make_shared<Sphere>(Point3(0, 0.5f, -2.0f), 0.5f), sphere_material);
	}

	camera.aspect_ratio = aspect_ratio;
	camera.initialize();

	Image image(image_width, image_height);

	const int samples_per_pixel = 200;

	cout << "Rendering with " << samples_per_pixel << " samples per pixel and " << 8 << " bounces..." << endl;
	
	auto start_time = std::chrono::high_resolution_clock::now();
	
	for (int y = 0; y < image_height; y++) {
		if (y % 50 == 0) {
			float progress = (y * 100.0f) / image_height;
			cout << "Progress: " << progress << "%" << endl;
		}
		for (int x = 0; x < image_width; x++) {
			Color pixel_color(0.0f, 0.0f, 0.0f);
			
			for (int s = 0; s < samples_per_pixel; s++) {
				float u = (x + random_float()) / float(image_width);
				float v = 1.0f - (y + random_float()) / float(image_height);
				Ray r = camera.get_ray(u, v);
				
				Color sample_color = ray_color(r, scene, 8);
				sample_color = sample_color.clamp01();
				pixel_color += sample_color;
			}
			
			pixel_color = Color(
				pixel_color.R() / samples_per_pixel,
				pixel_color.G() / samples_per_pixel,
				pixel_color.B() / samples_per_pixel
			);
			pixel_color = pixel_color.clamp01();
			
			image.SetPixel(x, y, pixel_color);
		}
	}
	
	auto end_time = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
	
	cout << "Rendering completed in " << duration.count() << " ms (" 
	     << (duration.count() / 1000.0) << " seconds)" << endl;

	cout << "Writing to file..." << endl;
	image.WriteFile("./output/test.png");

	return 0;
}
