#pragma once

#include "core/Integrator.hpp"
#include "core/Environment.hpp"

namespace raylabs {

class PathTracer : public Integrator {
   public:
    PathTracer() = default;
    ~PathTracer() override = default;

    /// Compute the color along a ray using path tracing
    Color trace(const Ray& ray, const Scene& scene, int max_depth) const override;

   private:
    /// Recursive ray color computation
    Color ray_color(const Ray& ray, const Scene& scene, int depth) const;
};

}  // namespace raylabs

