#pragma once

namespace raylabs {

class Sampler {
   public:
    /// Generate a random float in [0, 1)
    static float random_float();

    /// Generate a random float in [min, max)
    static float random_float(float min, float max);

   private:
    static unsigned int next_seed();
};

}  // namespace raylabs

