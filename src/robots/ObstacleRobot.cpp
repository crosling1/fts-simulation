#include "robots/ObstacleRobot.h"

#include <iostream>

ObstacleRobot::ObstacleRobot(double x, double y, double angle) : Robot(x, y, angle) {}

void ObstacleRobot::update() {
    rotate(1.0);
    moveForward(0.03);
}

void ObstacleRobot::printType() const {
    std::cout << "ObstacleRobot\n";
}
