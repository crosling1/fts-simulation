#pragma once

#include "robots/RobotState.h"
#include "raylib.h"

#include <string_view>

struct RobotRenderData {
    Vector2 position;
    float angleDegrees;
    float radius;
    float proximityDetectionRadius;
    RobotState state;
    bool carryingItem;
    std::string_view typeName;
};
