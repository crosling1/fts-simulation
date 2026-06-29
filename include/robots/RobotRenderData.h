#pragma once

#include "robots/RobotState.h"
#include "simulation/Geometry.h"

#include <string_view>

struct RobotRenderData {
    Vec2 position;
    float angleDegrees;
    float radius;
    float proximityDetectionRadius;
    RobotState state;
    bool carryingItem;
    std::string_view typeName;
};
