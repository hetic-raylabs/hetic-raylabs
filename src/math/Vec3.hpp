#pragma once

#include <iostream>
#include "math_utils.hpp"

class Vec3 {
   public:
    float x, y, z;

    Vec3() : x(0), y(0), z(0) {}
    Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

    Vec3 operator-() const { return Vec3(-x, -y, -z); }

    Vec3 operator+(const Vec3& v) const { return Vec3(x + v.x, y + v.y, z + v.z); }

    Vec3 operator-(const Vec3& v) const { return Vec3(x - v.x, y - v.y, z - v.z); }

    Vec3 operator*(float t) const { return Vec3(x * t, y * t, z * t); }

    Vec3 operator/(float t) const { return Vec3(x / t, y / t, z / t); }

    Vec3& operator+=(const Vec3& v) {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }

    Vec3& operator*=(float t) {
        x *= t;
        y *= t;
        z *= t;
        return *this;
    }

    Vec3& operator/=(float t) { return *this *= (1.0f / t); }

    float operator[](int i) const {
        if (i == 0)
            return x;
        if (i == 1)
            return y;
        return z;
    }

    float& operator[](int i) {
        if (i == 0)
            return x;
        if (i == 1)
            return y;
        return z;
    }

    float length_squared() const { return x * x + y * y + z * z; }

    float length() const { return rtmath::sqrt(length_squared()); }
};

using Point3 = Vec3;
// using Color = Vec3;

inline Vec3 operator*(float t, const Vec3& v) {
    return v * t;
}

inline std::ostream& operator<<(std::ostream& out, const Vec3& v) {
    return out << v.x << ' ' << v.y << ' ' << v.z;
}

inline float dot(const Vec3& u, const Vec3& v) {
    return u.x * v.x + u.y * v.y + u.z * v.z;
}

inline Vec3 cross(const Vec3& u, const Vec3& v) {
    return Vec3(u.y * v.z - u.z * v.y, u.z * v.x - u.x * v.z, u.x * v.y - u.y * v.x);
}

inline Vec3 normalize(const Vec3& v) {
    float len = v.length();
    if (len > 0.0f) {
        return v / len;
    }
    return Vec3(0, 0, 0);
}
