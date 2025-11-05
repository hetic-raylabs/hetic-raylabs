#include <iostream>
#include "core/Camera.hpp"
#include "core/HitRecord.hpp"
#include "core/Ray.hpp"
#include "core/Scene.hpp"
#include "entities/Plane.hpp"
#include "math/Vec3.hpp"

#include "Color.hpp"
#include "Image.hpp"

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

int main() {
    int image_height = 512;
    int image_width = 512;

    Camera camera(Point3(0, 1, 3), Point3(0, 0.3f, 0), Vec3(0, 1, 0), 60.0f, float(image_width) / float(image_height));

    Scene scene;
    scene.add(std::make_shared<Plane>(Point3(0, 0, 0), Vec3(0, 1, 0)));

    Image image(image_width, image_height);

    for (int y = 0; y < image_height; y++) {
        for (int x = 0; x < image_width; x++) {
            float u = (x + 0.5f) / float(image_width);
            float v = 1.0f - (y + 0.5f) / float(image_height);
            Ray r = camera.get_ray(u, v);

            HitRecord rec{};
            if (scene.hit(r, 0.001f, 1e9f, rec)) {
                image.SetPixel(x, y, ground_color(rec.point));
            } else {
                image.SetPixel(x, y, sky_color(r.direction));
            }
        }
    }

    image.WriteFile("./output/test.png");
}
