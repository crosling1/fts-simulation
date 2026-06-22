#include "sensors/ProximitySensor.h"

ProximitySensor::ProximitySensor(float detectionRadius) : detectionRadius_(detectionRadius) {}

bool ProximitySensor::hasBlockingRobotNearby(
    Vector2 origin, const BlockingRobotManager& blockingRobotManager) const {
    return blockingRobotManager.hasActiveBlockingRobotNear(origin, detectionRadius_);
}

void ProximitySensor::drawScanArea(Vector2 origin) const {
    DrawCircleLines((int)origin.x, (int)origin.y, detectionRadius_, Fade(BLUE, 0.55f));
}

float ProximitySensor::getDetectionRadius(void) const {
    return detectionRadius_;
}
