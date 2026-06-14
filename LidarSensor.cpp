#include "LidarSensor.h"

LidarSensor::LidarSensor(float detectionRadius) : detectionRadius(detectionRadius) {}

bool LidarSensor::hasObstacleNearby(Vector2 origin, const ObstacleManager& obstacleManager) const {
    return obstacleManager.hasActiveObstacleNear(origin, detectionRadius);
}

void LidarSensor::drawScanArea(Vector2 origin) const {
    DrawCircleLines((int)origin.x, (int)origin.y, detectionRadius, Fade(BLUE, 0.55f));
}

float LidarSensor::getDetectionRadius(void) const {
    return detectionRadius;
}
