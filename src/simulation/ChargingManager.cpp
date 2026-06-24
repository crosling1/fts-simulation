#include "simulation/ChargingManager.h"

#include "robots/Robot.h"
#include "simulation/RobotRoutePlanner.h"
#include "simulation/map.h"

#include <vector>

namespace {
constexpr float chargeAfterDropoffThreshold = 10.0f;
constexpr float minimumBatteryAfterJob = 10.0f;
constexpr float batteryDrainPercentagePerPixel = 0.01f;
} // namespace

ChargingManager::ChargingManager(const LogisticsMap& logisticsMap) : logisticsMap_(logisticsMap) {}

bool ChargingManager::shouldStartChargingAfterDropoff(Robot& robot,
                                                      const RobotRoutePlanner& routePlanner,
                                                      Vector2 robotPosition) const {
    return shouldChargeAtOrBelow(robot, chargeAfterDropoffThreshold) ||
           !canCompleteNextDeliveryBeforeMinimumBattery(robot, routePlanner, robotPosition);
}

bool ChargingManager::shouldChargeAtOrBelow(Robot& robot, float thresholdPercentage) const {
    return robot.getBattery().getChargePercentage() <= thresholdPercentage;
}

bool ChargingManager::canCompleteNextDeliveryBeforeMinimumBattery(
    Robot& robot, const RobotRoutePlanner& routePlanner, Vector2 robotPosition) const {
    const Vector2 pickupDock = logisticsMap_.getLagerDockPosition(logisticsMap_.getPickupLagerId());
    const std::vector<Vector2> pickupPath = routePlanner.buildPathToPickup(robotPosition);
    const std::vector<Vector2> dropoffPath = routePlanner.buildPathToDropoff(pickupDock);
    const float estimatedDistance = routePlanner.calculatePathDistance(robotPosition, pickupPath) +
                                    routePlanner.calculatePathDistance(pickupDock, dropoffPath);

    const float estimatedBatteryAfterJob = robot.getBattery().getChargePercentage() -
                                           (estimatedDistance * batteryDrainPercentagePerPixel);

    return estimatedBatteryAfterJob > minimumBatteryAfterJob;
}
