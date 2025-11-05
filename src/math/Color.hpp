#pragma once
#include <algorithm>  // std::clamp
#include <cmath>      // for potential future ops
#include <iosfwd>     // std::ostream forward-declare

// Compact RGB color (float). No implicit alpha.
// Semantics:
// - Components are stored as-is; arithmetic does NOT clamp except operator+ (legacy behavior).
// - Use clamp01() before writing to 8-bit images.
class Color {
   public:
    // Construct black by default.
    constexpr Color() noexcept : r_(0.0f), g_(0.0f), b_(0.0f) {}
    constexpr Color(float r, float g, float b) noexcept : r_(r), g_(g), b_(b) {}
    ~Color() = default;

    // Accessors (const, noexcept). Keep legacy names R/G/B.
    [[nodiscard]] constexpr float R() const noexcept { return r_; }
    [[nodiscard]] constexpr float G() const noexcept { return g_; }
    [[nodiscard]] constexpr float B() const noexcept { return b_; }

    // Saturated addition (legacy behavior preserved).
    [[nodiscard]] constexpr Color operator+(const Color& o) const noexcept {
        return Color{std::clamp(r_ + o.r_, 0.0f, 1.0f), std::clamp(g_ + o.g_, 0.0f, 1.0f),
                     std::clamp(b_ + o.b_, 0.0f, 1.0f)};
    }

    // Assign / compound ops (non-saturating).
    Color& operator=(const Color& o) noexcept = default;
    Color& operator+=(const Color& o) noexcept {
        r_ += o.r_;
        g_ += o.g_;
        b_ += o.b_;
        return *this;
    }

    // Scalar multiplication (non-saturating).
    [[nodiscard]] constexpr Color operator*(float s) const noexcept {
        return Color{r_ * s, g_ * s, b_ * s};
    }
    Color& operator*=(float s) noexcept {
        r_ *= s;
        g_ *= s;
        b_ *= s;
        return *this;
    }
    friend constexpr Color operator*(float s, const Color& c) noexcept { return c * s; }

    // Utility: clamp each component to [0,1].
    [[nodiscard]] constexpr Color clamp01() const noexcept {
        return Color{std::clamp(r_, 0.0f, 1.0f), std::clamp(g_, 0.0f, 1.0f),
                     std::clamp(b_, 0.0f, 1.0f)};
    }

    // Streaming helper: "(r,g,b)"
    friend std::ostream& operator<<(std::ostream& os, const Color& c);

   private:
    float r_, g_, b_;
};
