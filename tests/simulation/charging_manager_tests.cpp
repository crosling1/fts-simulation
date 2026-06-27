#include "support/test_helpers.h"

#include <catch2/catch_test_macros.hpp>

#include "robots/WorkerRobot.h"
#include "simulation/ChargingManager.h"
#include "simulation/RobotRoutePlanner.h"
#include "simulation/SimConstants.h"
#include "simulation/map.h"

namespace {
Robot::Config RobotConfig() {
    return {
        {10.0f, 90.0f, 8.0f},
        {0.0f, 0.0f, 1000.0f},
    };
}

float EstimatedNextDeliveryBatteryUse(const LogisticsMap& logisticsMap,
                                      const RobotRoutePlanner& routePlanner,
                                      Vector2 robotPosition) {
    const Vector2 pickupDock = logisticsMap.getLagerDockPosition(logisticsMap.getPickupLagerId());
    const std::vector<Vector2> pickupPath = routePlanner.buildPathToPickup(robotPosition);
    const std::vector<Vector2> dropoffPath = routePlanner.buildPathToDropoff(pickupDock);

    return (routePlanner.calculatePathDistance(robotPosition, pickupPath) +
            routePlanner.calculatePathDistance(pickupDock, dropoffPath)) *
           SimConstants::Battery::kDrainPerPixel;
}
} // namespace

TEST_CASE("Charging starts after dropoff at or below charging threshold", "[ChargingManager]") {
    LogisticsMap logisticsMap;
    logisticsMap.init();
    const RobotRoutePlanner routePlanner(logisticsMap);
    const ChargingManager chargingManager(logisticsMap);
    WorkerRobot robot(logisticsMap.getLagerDockPosition(logisticsMap.getDeliveryLagerId()),
                      RobotConfig());

    robot.getBattery().setChargePercentage(SimConstants::Battery::kChargeAfterDropoffThreshold);

    CHECK(
        chargingManager.shouldStartChargingAfterDropoff(robot, routePlanner, robot.getPosition()));
}

TEST_CASE("Charging starts when next delivery would violate minimum battery", "[ChargingManager]") {
    LogisticsMap logisticsMap;
    logisticsMap.init();
    const RobotRoutePlanner routePlanner(logisticsMap);
    const ChargingManager chargingManager(logisticsMap);
    WorkerRobot robot(logisticsMap.getLagerDockPosition(logisticsMap.getDeliveryLagerId()),
                      RobotConfig());

    const float nextDeliveryUse =
        EstimatedNextDeliveryBatteryUse(logisticsMap, routePlanner, robot.getPosition());
    robot.getBattery().setChargePercentage(SimConstants::Battery::kMinimumAfterJob +
                                           nextDeliveryUse);

    CHECK(
        chargingManager.shouldStartChargingAfterDropoff(robot, routePlanner, robot.getPosition()));
}

TEST_CASE("Charging is skipped when battery can complete next delivery safely",
          "[ChargingManager]") {
    LogisticsMap logisticsMap;
    logisticsMap.init();
    const RobotRoutePlanner routePlanner(logisticsMap);
    const ChargingManager chargingManager(logisticsMap);
    WorkerRobot robot(logisticsMap.getLagerDockPosition(logisticsMap.getDeliveryLagerId()),
                      RobotConfig());

    const float nextDeliveryUse =
        EstimatedNextDeliveryBatteryUse(logisticsMap, routePlanner, robot.getPosition());
    robot.getBattery().setChargePercentage(SimConstants::Battery::kMinimumAfterJob +
                                           nextDeliveryUse + 1.0f);

    CHECK_FALSE(
        chargingManager.shouldStartChargingAfterDropoff(robot, routePlanner, robot.getPosition()));
}
