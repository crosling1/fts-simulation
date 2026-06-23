#pragma once

#include "robots/Robot.h"

struct RobotStatusSnapshot {
    Robot::State state;
    float batteryPercentage;
    bool emergencyStopActive;
};
