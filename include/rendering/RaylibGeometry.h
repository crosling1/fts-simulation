#pragma once

#include "simulation/Geometry.h"
#include "raylib.h"

[[nodiscard]] inline Vector2 ToRaylib(Vec2 value) {
    return {value.x, value.y};
}

[[nodiscard]] inline Rectangle ToRaylib(Rect value) {
    return {value.x, value.y, value.width, value.height};
}

[[nodiscard]] inline Vec2 ToCore(Vector2 value) {
    return {value.x, value.y};
}

[[nodiscard]] inline Rect ToCore(Rectangle value) {
    return {value.x, value.y, value.width, value.height};
}
