#pragma once

#include "simulation/SimConfig.h"

#include <optional>

class Robot;

class ChargingManager {
  public:
    explicit ChargingManager(const SimConfig& simConfig = SimConfig::Default());

    [[nodiscard]] bool
    shouldStartChargingAfterDropoff(const Robot& robot,
                                    std::optional<float> nextDeliveryDistance) const;

  private:
    [[nodiscard]] bool shouldChargeAtOrBelow(const Robot& robot, float thresholdPercentage) const;
    [[nodiscard]] bool
    canCompleteNextDeliveryBeforeMinimumBattery(const Robot& robot,
                                                std::optional<float> nextDeliveryDistance) const;

    const SimConfig& simConfig_;
};
