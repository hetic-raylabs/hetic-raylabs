#pragma once

#include "core/Ray.hpp"
#include "math/Color.hpp"

// Forward declarations
class Scene;

namespace raylabs {

class Integrator {
   public:
    virtual ~Integrator() = default;

    /// Compute the color along a ray
    /// @param ray The ray to trace
    /// @param scene The scene to render
    /// @param max_depth Maximum recursion depth
    /// @return The computed color
    virtual Color trace(const Ray& ray, const Scene& scene, int max_depth) const = 0;
};

}  // namespace raylabs
