#include "simulation/ChargingManager.h"

#include "robots/Robot.h"

ChargingManager::ChargingManager(const SimConfig& simConfig) : simConfig_(simConfig) {}

bool ChargingManager::shouldStartChargingAfterDropoff(
    const Robot& robot, std::optional<float> nextDeliveryDistance) const {
    return shouldChargeAtOrBelow(robot, simConfig_.lowBatteryThreshold) ||
           !canCompleteNextDeliveryBeforeMinimumBattery(robot, nextDeliveryDistance);
}

bool ChargingManager::shouldChargeAtOrBelow(const Robot& robot, float thresholdPercentage) const {
    return robot.getBattery().getChargePercentage() <= thresholdPercentage;
}

bool ChargingManager::canCompleteNextDeliveryBeforeMinimumBattery(
    const Robot& robot, std::optional<float> nextDeliveryDistance) const {
    if (!nextDeliveryDistance) {
        return false;
    }

    const float estimatedBatteryAfterJob =
        robot.getBattery().getChargePercentage() -
        (*nextDeliveryDistance * simConfig_.batteryDrainPerPixel);

    return estimatedBatteryAfterJob > simConfig_.emergencyBatteryThreshold;
}
