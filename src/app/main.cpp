#include <iostream>
#include "math/vec3.h"

int main() {
    Vec3 a(1, 2, 3);
    Vec3 b(4, 5, 6);
    Vec3 c = a + b;

    std::cout << "Vec3 test: " << a << " + " << b << " = " << c << std::endl;
    std::cout << "Length of c: " << c.length() << std::endl;

    return 0;
}
