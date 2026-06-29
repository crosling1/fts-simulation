#pragma once

#include "simulation/Geometry.h"

class BlockingRobotManager;

class ProximitySensor {
  public:
    explicit ProximitySensor(float detectionRadius);

    [[nodiscard]] bool
    hasBlockingRobotNearby(Vec2 origin, const BlockingRobotManager& blockingRobotManager) const;
    [[nodiscard]] float getDetectionRadius() const;

  private:
    float detectionRadius_;
};
