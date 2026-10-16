#include "vec2.hpp"

#include <cmath>

Vec2 add(Vec2 a, Vec2 b) {
    return { a.x + b.x, a.y + b.y };
}

Vec2 scale(Vec2 v, float s) {
    return { v.x * s, v.y * s };
}

float length(Vec2 v) {
    return std::sqrt(v.x * v.x + v.y * v.y);
}
