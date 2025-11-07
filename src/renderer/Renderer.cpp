#include "renderer/Renderer.hpp"
#include <chrono>
#include <iostream>
#include "core/PathTracer.hpp"
#include "math/Color.hpp"
#include "math/Vec3.hpp"

namespace raylabs {

Renderer::Renderer(const Scene& scene, const Camera& camera, const io::ImageDTO& image_config,
                   std::shared_ptr<Integrator> integrator)
    : scene_(scene),
      camera_(camera),
      image_config_(image_config),
      integrator_(integrator),
      image_(image_config.width, image_config.height) {
    if (!integrator_) {
        integrator_ = std::make_shared<PathTracer>();
    }
}

void Renderer::render() {
    std::cout << "Rendering image of size " << image_config_.width << "x" << image_config_.height
              << std::endl;
    std::cout << "Rendering with " << image_config_.samples << " samples per pixel and "
              << image_config_.max_depth << " bounces..." << std::endl;

    auto start_time = std::chrono::high_resolution_clock::now();

    for (int y = 0; y < image_config_.height; y++) {
        if (y % 50 == 0) {
            float progress = (y * 100.0f) / image_config_.height;
            std::cout << "Progress: " << progress << "%" << std::endl;
        }
        for (int x = 0; x < image_config_.width; x++) {
            Color pixel_color = render_pixel(x, y);
            image_.SetPixel(x, y, pixel_color);
        }
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    std::cout << "Rendering completed in " << duration.count() << " ms ("
              << (duration.count() / 1000.0) << " seconds)" << std::endl;

    std::cout << "Writing to file: " << image_config_.output_path << std::endl;
    image_.WriteFile(image_config_.output_path.c_str());
}

Color Renderer::render_pixel(int x, int y) const {
    Color pixel_color(0.0f, 0.0f, 0.0f);

    for (int s = 0; s < image_config_.samples; s++) {
        float u = (x + Sampler::random_float()) / float(image_config_.width);
        float v = 1.0f - (y + Sampler::random_float()) / float(image_config_.height);
        Ray r = camera_.get_ray(u, v);

        Color sample_color = integrator_->trace(r, scene_, image_config_.max_depth);
        sample_color = sample_color.clamp01();
        pixel_color += sample_color;
    }

    pixel_color =
        Color(pixel_color.R() / image_config_.samples, pixel_color.G() / image_config_.samples,
              pixel_color.B() / image_config_.samples);
    pixel_color = pixel_color.clamp01();

    return pixel_color;
}

}  // namespace raylabs
