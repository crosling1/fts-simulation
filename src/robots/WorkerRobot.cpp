#include "robots/WorkerRobot.h"

#include <iostream>

WorkerRobot::WorkerRobot(Pose startPose, Config config) : Robot(startPose, config) {}

WorkerRobot::WorkerRobot(const Vector2& startPosition, Config config)
    : Robot(startPosition, config) {}

void WorkerRobot::printType() const {
    std::cout << "WorkerRobot\n";
}
