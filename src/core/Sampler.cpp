#include "core/Sampler.hpp"

namespace raylabs {

unsigned int Sampler::next_seed() {
    static thread_local unsigned int seed = 12345;
    seed = seed * 1103515245 + 12345;
    seed = (seed << 16) ^ (seed >> 16);
    seed = seed * 224250251 + 198491317;
    seed = (seed << 13) ^ (seed >> 19);
    return seed;
}

float Sampler::random_float() {
    return (next_seed() & 0x7FFFFFFF) / 2147483647.0f;
}

float Sampler::random_float(float min, float max) {
    return min + (max - min) * random_float();
}

}  // namespace raylabs
