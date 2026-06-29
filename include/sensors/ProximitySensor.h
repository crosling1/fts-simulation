#pragma once

#include "raylib.h"

class BlockingRobotManager;

// Helper for radius-based proximity checks.
class ProximitySensor {
  public:
    explicit ProximitySensor(float detectionRadius);

    [[nodiscard]] bool
    hasBlockingRobotNearby(Vector2 origin, const BlockingRobotManager& blockingRobotManager) const;
    [[nodiscard]] float getDetectionRadius() const;

  private:
    float detectionRadius_;
};
