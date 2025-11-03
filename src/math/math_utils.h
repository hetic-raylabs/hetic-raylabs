#pragma once

namespace rtmath {

inline float abs(float x) {
    return x < 0.0f ? -x : x;
}

inline float min(float a, float b) {
    return a < b ? a : b;
}

inline float max(float a, float b) {
    return a > b ? a : b;
}

inline float sqrt(float x) {
    if (x <= 0.0f)
        return 0.0f;

    float guess = x;
    for (int i = 0; i < 10; ++i) {
        guess = (guess + x / guess) * 0.5f;
    }
    return guess;
}

}  // namespace rtmath
