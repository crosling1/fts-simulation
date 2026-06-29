#pragma once

#include "raylib.h"

#include <cmath>

namespace math {
inline constexpr float kPi = 3.14159265358979323846f;
inline constexpr float kRadToDeg = 57.29577951308232f;
inline constexpr float kDegToRad = 0.017453292519943295f;

[[nodiscard]] inline float distanceSq(Vector2 from, Vector2 to) {
    const float deltaX = to.x - from.x;
    const float deltaY = to.y - from.y;

    return (deltaX * deltaX) + (deltaY * deltaY);
}

[[nodiscard]] inline float distance(Vector2 from, Vector2 to) {
    return std::sqrt(distanceSq(from, to));
}
} // namespace math
