#pragma once

#include "core/Ray.hpp"
#include "math/Vec3.hpp"
#include "math/math_utils.hpp"

class Camera {
   public:
    Point3 position;
    Point3 look_at;
    Vec3 up;
    float fov;
    float aspect_ratio;

   private:
    Vec3 u, v, w;
    Vec3 horizontal;
    Vec3 vertical;
    Point3 lower_left_corner;
    bool initialized;

   public:
    Camera()
        : position(0, 0, 0),
          look_at(0, 0, -1),
          up(0, 1, 0),
          fov(90.0f),
          aspect_ratio(16.0f / 9.0f),
          initialized(false) {}

    Camera(const Point3& pos, const Point3& target, const Vec3& up_vec, float vertical_fov,
           float aspect)
        : position(pos),
          look_at(target),
          up(up_vec),
          fov(vertical_fov),
          aspect_ratio(aspect),
          initialized(false) {
        initialize();
    }

    void initialize() {
        const float pi = 3.14159265359f;
        float theta = fov * pi / 180.0f;
        float h = raylabs::tan(theta / 2.0f);
        float viewport_height = 2.0f * h;
        float viewport_width = aspect_ratio * viewport_height;

        w = normalize(position - look_at);
        u = normalize(cross(up, w));
        v = cross(w, u);

        horizontal = viewport_width * u;
        vertical = viewport_height * v;
        lower_left_corner = position - horizontal / 2.0f - vertical / 2.0f - w;

        initialized = true;
    }

    Ray get_ray(float s, float t) const {
        return Ray(position, lower_left_corner + s * horizontal + t * vertical - position);
    }
};
