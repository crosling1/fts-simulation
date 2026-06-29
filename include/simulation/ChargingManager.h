#pragma once

#include "simulation/SimConfig.h"

#include "raylib.h"

class ILogisticsMap;
class Robot;
class RobotRoutePlanner;

class ChargingManager {
  public:
    explicit ChargingManager(const ILogisticsMap& logisticsMap,
                             const SimConfig& simConfig = SimConfig::Default());

    [[nodiscard]] bool shouldStartChargingAfterDropoff(const Robot& robot,
                                                       const RobotRoutePlanner& routePlanner,
                                                       Vector2 robotPosition) const;

  private:
    [[nodiscard]] bool shouldChargeAtOrBelow(const Robot& robot, float thresholdPercentage) const;
    [[nodiscard]] bool canCompleteNextDeliveryBeforeMinimumBattery(
        const Robot& robot, const RobotRoutePlanner& routePlanner, Vector2 robotPosition) const;

    const ILogisticsMap& logisticsMap_;
    const SimConfig& simConfig_;
};
