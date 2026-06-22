#include "robots/WorkerRobot.h"

#include "sensors/LidarSensor.h"

#include <iostream>
#include <memory>

WorkerRobot::WorkerRobot(double x, double y, double angle) : Robot(x, y, angle) {
    addSensor(std::make_unique<LidarSensor>(0.3, 0.0, 0.0));
    addSensor(std::make_unique<LidarSensor>(-0.3, 0.0, 180.0));
}

WorkerRobot::WorkerRobot(const Vector2& startPosition, Config config)
    : Robot(startPosition, config) {
    addSensor(std::make_unique<LidarSensor>(0.3, 0.0, 0.0));
    addSensor(std::make_unique<LidarSensor>(-0.3, 0.0, 180.0));
}

void WorkerRobot::printType() const {
    std::cout << "WorkerRobot\n";
}
