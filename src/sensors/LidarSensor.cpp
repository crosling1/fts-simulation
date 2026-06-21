#include "sensors/LidarSensor.h"

#include "robots/Robot.h"

#include <cmath>

namespace {
constexpr double pi = 3.14159265358979323846;

double DegreesToRadians(double degree) {
    return degree * pi / 180.0;
}
} // namespace

LidarSensor::LidarSensor(float detectionRadius)
    : offsetX_(0.0), offsetY_(0.0), offsetAngle_(0.0), distance_(0.0),
      detectionRadius_(detectionRadius) {}

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
LidarSensor::LidarSensor(double offsetX, double offsetY, double offsetAngle)
    : offsetX_(offsetX), offsetY_(offsetY), offsetAngle_(offsetAngle), distance_(0.0),
      detectionRadius_(0.0f) {}

void LidarSensor::update(const Robot& robot) {
    const double robotAngleRadians = DegreesToRadians(robot.angle());
    const double sensorX = robot.x() + (std::cos(robotAngleRadians) * offsetX_) -
                           (std::sin(robotAngleRadians) * offsetY_);
    const double sensorY = robot.y() + (std::sin(robotAngleRadians) * offsetX_) +
                           (std::cos(robotAngleRadians) * offsetY_);
    const double sensorAngle = robot.angle() + offsetAngle_;

    (void)sensorX;
    (void)sensorY;
    (void)sensorAngle;

    distance_ = 10.0;
}

double LidarSensor::value() const {
    return distance_;
}

bool LidarSensor::hasObstacleNearby(Vector2 origin, const ObstacleManager& obstacleManager) const {
    return obstacleManager.hasActiveObstacleNear(origin, detectionRadius_);
}

void LidarSensor::drawScanArea(Vector2 origin) const {
    DrawCircleLines((int)origin.x, (int)origin.y, detectionRadius_, Fade(BLUE, 0.55f));
}

float LidarSensor::getDetectionRadius(void) const {
    return detectionRadius_;
}
