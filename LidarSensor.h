#ifndef LIDAR_SENSOR_H
#define LIDAR_SENSOR_H

#include "ObstacleManager.h"
#include "raylib.h"

class LidarSensor {
  public:
    explicit LidarSensor(float detectionRadius);

    bool hasObstacleNearby(Vector2 origin, const ObstacleManager& obstacleManager) const;
    void drawScanArea(Vector2 origin) const;
    float getDetectionRadius(void) const;

  private:
    float detectionRadius;
};

#endif // LIDAR_SENSOR_H
