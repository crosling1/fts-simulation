#ifndef PROXIMITY_SENSOR_H
#define PROXIMITY_SENSOR_H

#include "raylib.h"

class BlockingRobotManager;

// Helper for radius-based proximity checks.
class ProximitySensor {
  public:
    explicit ProximitySensor(float detectionRadius);

    bool hasBlockingRobotNearby(Vector2 origin,
                                const BlockingRobotManager& blockingRobotManager) const;
    void drawScanArea(Vector2 origin) const;
    float getDetectionRadius(void) const;

  private:
    float detectionRadius_;
};

#endif // PROXIMITY_SENSOR_H
