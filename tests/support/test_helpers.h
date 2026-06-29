#pragma once

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "raylib.h"

#include <cmath>
#include <vector>

namespace test {
constexpr float epsilon = 0.001f;

inline bool AlmostEqual(float left, float right) noexcept {
    return std::fabs(left - right) <= epsilon;
}

inline void CheckVectorNear(Vector2 actual, Vector2 expected) {
    CAPTURE(actual.x, actual.y, expected.x, expected.y);
    CHECK(actual.x == Catch::Approx(expected.x).margin(epsilon));
    CHECK(actual.y == Catch::Approx(expected.y).margin(epsilon));
}

inline bool PathContainsPoint(const std::vector<Vector2>& path, Vector2 expectedPoint) {
    for (Vector2 waypoint : path) {
        if (AlmostEqual(waypoint.x, expectedPoint.x) && AlmostEqual(waypoint.y, expectedPoint.y)) {
            return true;
        }
    }

    return false;
}
} // namespace test
