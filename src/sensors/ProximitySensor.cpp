#include "sensors/ProximitySensor.h"

#include "simulation/BlockingRobotManager.h"

ProximitySensor::ProximitySensor(float detectionRadius) : detectionRadius_(detectionRadius) {}

bool ProximitySensor::hasBlockingRobotNearby(
    Vec2 origin, const BlockingRobotManager& blockingRobotManager) const {
    return blockingRobotManager.hasActiveBlockingRobotNear(origin, detectionRadius_);
}

float ProximitySensor::getDetectionRadius() const {
    return detectionRadius_;
}
