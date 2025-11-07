#include "core/PathTracer.hpp"
#include "core/HitRecord.hpp"
#include "core/Scene.hpp"
#include "materials/Material.hpp"
#include "math/Color.hpp"

namespace raylabs {

Color PathTracer::trace(const Ray& ray, const Scene& scene, int max_depth) const {
    return ray_color(ray, scene, max_depth);
}

Color PathTracer::ray_color(const Ray& ray, const Scene& scene, int depth) const {
    if (depth <= 0) {
        return Color(0.0f, 0.0f, 0.0f);
    }

    HitRecord rec;
    if (scene.hit(ray, 0.001f, 1e9f, rec)) {
        if (rec.material) {
            Ray scattered;
            Color attenuation;
            if (rec.material->scatter(ray, rec, attenuation, scattered)) {
                Color recurse_color = ray_color(scattered, scene, depth - 1);
                return Color(attenuation.R() * recurse_color.R(),
                             attenuation.G() * recurse_color.G(),
                             attenuation.B() * recurse_color.B());
            }
            return Color(0, 0, 0);
        }
        return Color(0.5f, 0.5f, 0.5f);
    }
    return Environment::sky_color(ray.direction);
}

}  // namespace raylabs
