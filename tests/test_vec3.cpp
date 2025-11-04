#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include "math/Vec3.hpp"

TEST_CASE("Vec3 construction") {
    Vec3 v;
    CHECK(v.x == 0.0f);
    CHECK(v.y == 0.0f);
    CHECK(v.z == 0.0f);

    Vec3 v2(1, 2, 3);
    CHECK(v2.x == 1.0f);
    CHECK(v2.y == 2.0f);
    CHECK(v2.z == 3.0f);
}

TEST_CASE("Vec3 addition") {
    Vec3 a(1, 2, 3);
    Vec3 b(4, 5, 6);
    Vec3 c = a + b;
    CHECK(c.x == 5.0f);
    CHECK(c.y == 7.0f);
    CHECK(c.z == 9.0f);
}

TEST_CASE("Vec3 subtraction") {
    Vec3 a(4, 5, 6);
    Vec3 b(1, 2, 3);
    Vec3 c = a - b;
    CHECK(c.x == 3.0f);
    CHECK(c.y == 3.0f);
    CHECK(c.z == 3.0f);
}

TEST_CASE("Vec3 scalar multiplication") {
    Vec3 v(1, 2, 3);
    Vec3 result = v * 2.0f;
    CHECK(result.x == 2.0f);
    CHECK(result.y == 4.0f);
    CHECK(result.z == 6.0f);

    Vec3 result2 = 3.0f * v;
    CHECK(result2.x == 3.0f);
    CHECK(result2.y == 6.0f);
    CHECK(result2.z == 9.0f);
}

TEST_CASE("Vec3 scalar division") {
    Vec3 v(2, 4, 6);
    Vec3 result = v / 2.0f;
    CHECK(result.x == 1.0f);
    CHECK(result.y == 2.0f);
    CHECK(result.z == 3.0f);
}

TEST_CASE("Vec3 negation") {
    Vec3 v(1, -2, 3);
    Vec3 neg = -v;
    CHECK(neg.x == -1.0f);
    CHECK(neg.y == 2.0f);
    CHECK(neg.z == -3.0f);
}

TEST_CASE("Vec3 dot product") {
    Vec3 a(1, 0, 0);
    Vec3 b(0, 1, 0);
    CHECK(dot(a, b) == 0.0f);

    Vec3 c(1, 2, 3);
    Vec3 d(4, 5, 6);
    CHECK(dot(c, d) == 32.0f);
}

TEST_CASE("Vec3 cross product") {
    Vec3 x(1, 0, 0);
    Vec3 y(0, 1, 0);
    Vec3 z = cross(x, y);
    CHECK(z.x == 0.0f);
    CHECK(z.y == 0.0f);
    CHECK(z.z == 1.0f);
}

TEST_CASE("Vec3 length") {
    Vec3 v(3, 4, 0);
    CHECK(v.length() == doctest::Approx(5.0f).epsilon(0.001));

    Vec3 v2(1, 1, 1);
    CHECK(v2.length() == doctest::Approx(1.732f).epsilon(0.001));
}

TEST_CASE("Vec3 normalize") {
    Vec3 v(3, 4, 0);
    Vec3 n = normalize(v);
    CHECK(n.length() == doctest::Approx(1.0f).epsilon(0.001));
    CHECK(n.x == doctest::Approx(0.6f).epsilon(0.001));
    CHECK(n.y == doctest::Approx(0.8f).epsilon(0.001));
}

TEST_CASE("Custom sqrt function") {
    CHECK(raylabs::sqrt(0.0f) == doctest::Approx(0.0f));
    CHECK(raylabs::sqrt(1.0f) == doctest::Approx(1.0f));
    CHECK(raylabs::sqrt(4.0f) == doctest::Approx(2.0f));
    CHECK(raylabs::sqrt(9.0f) == doctest::Approx(3.0f));
    CHECK(raylabs::sqrt(25.0f) == doctest::Approx(5.0f));
    CHECK(raylabs::sqrt(2.0f) == doctest::Approx(1.414213f).epsilon(0.001));
}
