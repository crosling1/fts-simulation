#pragma once

#include "raylib.h"

class BlockingRobotManager;

class ProximitySensor {
  public:
    explicit ProximitySensor(float detectionRadius);

    [[nodiscard]] bool
    hasBlockingRobotNearby(Vector2 origin, const BlockingRobotManager& blockingRobotManager) const;
    [[nodiscard]] float getDetectionRadius() const;

  private:
    float detectionRadius_;
};
