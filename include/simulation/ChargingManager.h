#pragma once

#include "simulation/SimConfig.h"

#include "raylib.h"

class LogisticsMap;
class Robot;
class RobotRoutePlanner;

class ChargingManager {
  public:
    explicit ChargingManager(const LogisticsMap& logisticsMap,
                             SimConfig simConfig = SimConfig::Default());

    [[nodiscard]] bool shouldStartChargingAfterDropoff(const Robot& robot,
                                                       const RobotRoutePlanner& routePlanner,
                                                       Vector2 robotPosition) const;

  private:
    [[nodiscard]] bool shouldChargeAtOrBelow(const Robot& robot, float thresholdPercentage) const;
    [[nodiscard]] bool canCompleteNextDeliveryBeforeMinimumBattery(
        const Robot& robot, const RobotRoutePlanner& routePlanner, Vector2 robotPosition) const;

    const LogisticsMap& logisticsMap_;
    SimConfig simConfig_;
};
