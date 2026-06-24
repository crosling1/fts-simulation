#pragma once

#include "raylib.h"

class LogisticsMap;
class Robot;
class RobotRoutePlanner;

class ChargingManager {
  public:
    explicit ChargingManager(const LogisticsMap& logisticsMap);

    bool shouldStartChargingAfterDropoff(Robot& robot, const RobotRoutePlanner& routePlanner,
                                         Vector2 robotPosition) const;

  private:
    bool shouldChargeAtOrBelow(Robot& robot, float thresholdPercentage) const;
    bool canCompleteNextDeliveryBeforeMinimumBattery(Robot& robot,
                                                     const RobotRoutePlanner& routePlanner,
                                                     Vector2 robotPosition) const;

    const LogisticsMap& logisticsMap_;
};
