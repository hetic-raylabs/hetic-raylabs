#include "math/Color.hpp"
#include <ostream>

// Stream as "(r,g,b)" with raw floats.
// Note: if you need sRGB encoding and gamma, do it at image write time.
std::ostream& operator<<(std::ostream& os, const Color& c) {
    os << '(' << c.R() << ',' << c.G() << ',' << c.B() << ')';
    return os;
}
