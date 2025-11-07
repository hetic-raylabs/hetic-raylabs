#pragma once

#include <memory>
#include <string>
#include "core/Camera.hpp"
#include "core/Scene.hpp"
#include "core/Integrator.hpp"
#include "core/Sampler.hpp"
#include "image/Image.hpp"
#include "io/JsonSceneLoader.hpp"

namespace raylabs {

class Renderer {
   public:
    Renderer(const Scene& scene, const Camera& camera, const io::ImageDTO& image_config,
             std::shared_ptr<Integrator> integrator);

    /// Render the scene and write to output file
    void render();

   private:
    const Scene& scene_;
    const Camera& camera_;
    io::ImageDTO image_config_;
    std::shared_ptr<Integrator> integrator_;
    Image image_;

    /// Render a single pixel with antialiasing
    Color render_pixel(int x, int y) const;
};

}  // namespace raylabs

