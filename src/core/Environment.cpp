#include "core/Environment.hpp"
#include "math/Math_utils.hpp"

namespace raylabs {

Color Environment::sky_color(const Vec3& direction) {
    Vec3 normalized_dir = normalize(direction);
    float t = 0.5f * (normalized_dir.y + 1.0f);
    
    float r = (1.0f - t) * sky_bottom_r + t * sky_top_r;
    float g = (1.0f - t) * sky_bottom_g + t * sky_top_g;
    float b = (1.0f - t) * sky_bottom_b + t * sky_top_b;
    
    return Color(r, g, b);
}

}  // namespace raylabs

