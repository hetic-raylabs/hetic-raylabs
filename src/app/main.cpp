#include <iostream>
#include "core/Camera.hpp"
#include "core/HitRecord.hpp"
#include "core/Ray.hpp"
#include "math/Vec3.hpp"

int main() {
    std::cout << "=== Vec3 Test ===" << std::endl;
    Vec3 a(1, 2, 3);
    Vec3 b(4, 5, 6);
    Vec3 c = a + b;
    std::cout << "Vec3 test: " << a << " + " << b << " = " << c << std::endl;
    std::cout << "Length of c: " << c.length() << std::endl;

    std::cout << "\n=== Ray Test ===" << std::endl;
    Ray ray(Point3(0, 0, 0), Vec3(1, 0, 0));
    std::cout << "Ray origin: " << ray.origin << std::endl;
    std::cout << "Ray direction: " << ray.direction << std::endl;
    std::cout << "Point at t=5: " << ray.at(5.0f) << std::endl;

    std::cout << "\n=== HitRecord Test ===" << std::endl;
    HitRecord rec;
    rec.point = Point3(1, 2, 3);
    rec.normal = Vec3(0, 1, 0);
    rec.t = 5.0f;
    std::cout << "Hit point: " << rec.point << std::endl;
    std::cout << "Normal: " << rec.normal << std::endl;
    std::cout << "Distance t: " << rec.t << std::endl;

    std::cout << "\n=== Camera Test ===" << std::endl;
    Point3 camera_pos(0, 0, 0);
    Point3 look_at(0, 0, -1);
    Vec3 up(0, 1, 0);
    float fov = 90.0f;
    float aspect_ratio = 16.0f / 9.0f;

    Camera camera(camera_pos, look_at, up, fov, aspect_ratio);
    std::cout << "Camera position: " << camera.position << std::endl;
    std::cout << "Camera look_at: " << camera.look_at << std::endl;
    std::cout << "Camera FOV: " << camera.fov << std::endl;
    std::cout << "Camera aspect ratio: " << camera.aspect_ratio << std::endl;

    std::cout << "\n=== Camera Ray Generation ===" << std::endl;
    Ray center_ray = camera.get_ray(0.5f, 0.5f);
    std::cout << "Center ray origin: " << center_ray.origin << std::endl;
    std::cout << "Center ray direction: " << center_ray.direction << std::endl;

    Ray corner_ray = camera.get_ray(0.0f, 0.0f);
    std::cout << "Bottom-left ray origin: " << corner_ray.origin << std::endl;
    std::cout << "Bottom-left ray direction: " << corner_ray.direction << std::endl;

    return 0;
}
