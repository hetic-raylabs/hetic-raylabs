#pragma once

namespace raylabs {

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

inline float tan(float x) {
    const float pi = 3.14159265359f;
    x = x - ((int)(x / (2.0f * pi))) * (2.0f * pi);

    float result = x;
    float x2 = x * x;
    float numerator = x;
    float denominator = 1.0f;

    for (int i = 1; i < 8; ++i) {
        numerator *= x2;
        denominator *= (2.0f * i) * (2.0f * i + 1.0f);
        float term = numerator / denominator;
        if (i % 2 == 0) {
            result += term;
        } else {
            result += term;
        }
    }
    return result;
}

}  // namespace raylabs
