#ifndef LIDAR_SENSOR_H
#define LIDAR_SENSOR_H

#include "sensors/Sensor.h"
#include "simulation/ObstacleManager.h"
#include "raylib.h"

class LidarSensor : public Sensor {
  public:
    explicit LidarSensor(float detectionRadius);
    LidarSensor(double offsetX, double offsetY, double offsetAngle);

    void update(const Robot& robot) override;
    double value() const override;
    bool hasObstacleNearby(Vector2 origin, const ObstacleManager& obstacleManager) const;
    void drawScanArea(Vector2 origin) const;
    float getDetectionRadius(void) const;

  private:
    double offsetX_;
    double offsetY_;
    double offsetAngle_;
    double distance_;
    float detectionRadius_;
};

#endif // LIDAR_SENSOR_H
