#include "sensors/ProximitySensor.h"

#include "simulation/BlockingRobotManager.h"

ProximitySensor::ProximitySensor(float detectionRadius) : detectionRadius_(detectionRadius) {}

bool ProximitySensor::hasBlockingRobotNearby(
    Vector2 origin, const BlockingRobotManager& blockingRobotManager) const {
    return blockingRobotManager.hasActiveBlockingRobotNear(origin, detectionRadius_);
}

void ProximitySensor::drawScanArea(Vector2 origin) const {
    DrawCircleLines(static_cast<int>(origin.x), static_cast<int>(origin.y), detectionRadius_,
                    Fade(BLUE, 0.55f));
}

float ProximitySensor::getDetectionRadius() const {
    return detectionRadius_;
}
