#ifndef TEST_HELPERS_H
#define TEST_HELPERS_H

#include "raylib.h"

#include <cmath>
#include <stdexcept>
#include <string>
#include <vector>

namespace test {
constexpr float epsilon = 0.001f;

inline bool AlmostEqual(float left, float right) {
    return std::fabs(left - right) <= epsilon;
}

inline void Expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

inline void ExpectVectorNear(Vector2 actual, Vector2 expected, const std::string& message) {
    if (!AlmostEqual(actual.x, expected.x) || !AlmostEqual(actual.y, expected.y)) {
        throw std::runtime_error(message + " expected (" + std::to_string(expected.x) + ", " +
                                 std::to_string(expected.y) + ") got (" + std::to_string(actual.x) +
                                 ", " + std::to_string(actual.y) + ")");
    }
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

#endif // TEST_HELPERS_H
