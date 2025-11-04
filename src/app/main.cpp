#include <iostream>
#include "core/HitRecord.hpp"
#include "core/Ray.hpp"
#include "math/Vec3.hpp"

#include "Color.hpp"
#include "Image.hpp"

using namespace std;

int main() {
    // Code de la fonctionnalité math
    /*
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
    */

    Color red(1, 0, 0);
    Color green(0, 1, 0);
    Color blue(0, 0, 1);
    Color black;

    int image_height = 512;
    int image_width = 512;

    cout << "Red : " << red << std::endl;
    cout << "Green : " << green << std::endl;
    cout << "Blue : " << blue << std::endl;
    cout << "Black : " << black << std::endl;

    Color yellow = red + green;

    cout << "Yellow : " << yellow << std::endl;

    // Create an image in memory, and fill it with yellow
    // Image image(512, 512, yellow);
    Image image(image_width, image_height);

    // Make a red square on the top left of the image
    for (int y = 0; y < image_height; y++) {
      for (int x = 0; x < image_width; x++) {
        // image.SetPixel(x, y, Color(1, 1, 1));
        double r = double(x) / (image_width - 1);
        double g = double(y) / (image_height - 1);
        double b = 0.0;
        image.SetPixel(x, y, Color(r, g, b));
      }
    }

    image.WriteFile("./output/test.png");

}
