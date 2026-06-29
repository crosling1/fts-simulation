#pragma once

#include "simulation/Geometry.h"

#include <cmath>

namespace math {
inline constexpr float kPi = 3.14159265358979323846f;
inline constexpr float kRadToDeg = 57.29577951308232f;
inline constexpr float kDegToRad = 0.017453292519943295f;

[[nodiscard]] inline float distanceSq(Vec2 from, Vec2 to) {
    const float deltaX = to.x - from.x;
    const float deltaY = to.y - from.y;

    return (deltaX * deltaX) + (deltaY * deltaY);
}

[[nodiscard]] inline float distance(Vec2 from, Vec2 to) {
    return std::sqrt(distanceSq(from, to));
}
} // namespace math
