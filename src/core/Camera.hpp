#pragma once

#include "core/Ray.hpp"
#include "math/Vec3.hpp"

class Camera {
   public:
    Point3 position;
    Point3 look_at;
    Vec3 up;
    float fov;
    float aspect_ratio;

    Camera()
        : position(0, 0, 0),
          look_at(0, 0, -1),
          up(0, 1, 0),
          fov(90.0f),
          aspect_ratio(16.0f / 9.0f) {}

    Camera(const Point3& pos, const Point3& target, const Vec3& up_vec, float vertical_fov,
           float aspect)
        : position(pos), look_at(target), up(up_vec), fov(vertical_fov), aspect_ratio(aspect) {}
};
