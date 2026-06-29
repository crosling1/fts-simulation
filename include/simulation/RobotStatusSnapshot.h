#pragma once

#include "robots/RobotState.h"

struct RobotStatusSnapshot {
    RobotState state;
    float batteryPercentage;
    bool emergencyStopActive;
};
