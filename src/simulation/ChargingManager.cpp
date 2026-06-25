#include "simulation/ChargingManager.h"
#include "simulation/SimConstants.h"

#include "robots/Robot.h"
#include "simulation/RobotRoutePlanner.h"
#include "simulation/map.h"

#include <vector>

ChargingManager::ChargingManager(const LogisticsMap& logisticsMap) : logisticsMap_(logisticsMap) {}

bool ChargingManager::shouldStartChargingAfterDropoff(const Robot& robot,
                                                      const RobotRoutePlanner& routePlanner,
                                                      Vector2 robotPosition) const {
    return shouldChargeAtOrBelow(robot, SimConstants::kChargeAfterDropoffThreshold) ||
           !canCompleteNextDeliveryBeforeMinimumBattery(robot, routePlanner, robotPosition);
}

bool ChargingManager::shouldChargeAtOrBelow(const Robot& robot, float thresholdPercentage) const {
    return robot.getBattery().getChargePercentage() <= thresholdPercentage;
}

bool ChargingManager::canCompleteNextDeliveryBeforeMinimumBattery(
    const Robot& robot, const RobotRoutePlanner& routePlanner, Vector2 robotPosition) const {
    const Vector2 pickupDock = logisticsMap_.getLagerDockPosition(logisticsMap_.getPickupLagerId());
    const std::vector<Vector2> pickupPath = routePlanner.buildPathToPickup(robotPosition);
    const std::vector<Vector2> dropoffPath = routePlanner.buildPathToDropoff(pickupDock);
    const float estimatedDistance = routePlanner.calculatePathDistance(robotPosition, pickupPath) +
                                    routePlanner.calculatePathDistance(pickupDock, dropoffPath);

    const float estimatedBatteryAfterJob =
        robot.getBattery().getChargePercentage() -
        (estimatedDistance * SimConstants::kBatteryDrainPerPixel);

    return estimatedBatteryAfterJob > SimConstants::kMinimumBatteryAfterJob;
}
