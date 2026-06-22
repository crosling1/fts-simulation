#include "robots/WorkerRobot.h"

#include <iostream>

WorkerRobot::WorkerRobot(double x, double y, double angle) : Robot(x, y, angle) {}

WorkerRobot::WorkerRobot(const Vector2& startPosition, Config config)
    : Robot(startPosition, config) {}

void WorkerRobot::printType() const {
    std::cout << "WorkerRobot\n";
}
