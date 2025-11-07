#pragma once

#include "math/Color.hpp"
#include "math/Vec3.hpp"

namespace raylabs {

class Environment {
   public:
    /// Compute sky color based on ray direction (gradient sky)
    static Color sky_color(const Vec3& direction);

   private:
    // Gradient sky parameters
    static constexpr float sky_top_r = 0.5f;
    static constexpr float sky_top_g = 0.7f;
    static constexpr float sky_top_b = 1.0f;
    static constexpr float sky_bottom_r = 1.0f;
    static constexpr float sky_bottom_g = 1.0f;
    static constexpr float sky_bottom_b = 1.0f;
};

}  // namespace raylabs

