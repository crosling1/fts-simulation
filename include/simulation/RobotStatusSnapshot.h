#ifndef ROBOT_STATUS_SNAPSHOT_H
#define ROBOT_STATUS_SNAPSHOT_H

#include "robots/Robot.h"

struct RobotStatusSnapshot {
    Robot::State state;
    float batteryPercentage;
    bool emergencyStopActive;
};

#endif // ROBOT_STATUS_SNAPSHOT_H
