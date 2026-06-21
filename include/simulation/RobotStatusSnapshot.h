#ifndef ROBOT_STATUS_SNAPSHOT_H
#define ROBOT_STATUS_SNAPSHOT_H

#include "robots/Robot.h"

struct RobotStatusSnapshot {
    Robot::State state;
    float batteryPercentage;
};

#endif // ROBOT_STATUS_SNAPSHOT_H
