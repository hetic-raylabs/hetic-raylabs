#include <iostream>
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

    return 0;
}
