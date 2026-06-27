#include "support/test_helpers.h"

#include <catch2/catch_test_macros.hpp>

#include "robots/WorkerRobot.h"
#include "simulation/ChargingManager.h"
#include "simulation/RobotRoutePlanner.h"
#include "simulation/SimConfig.h"
#include "simulation/map.h"

namespace {
constexpr float kBatteryTestMargin = 0.01f;

Robot::Config RobotConfig() {
    return {
        {10.0f, 90.0f, 8.0f},
        {0.0f, 0.0f, 1000.0f},
    };
}

void SetRobotBatteryPercentage(WorkerRobot& robot, float percentage) {
    const SimConfig simConfig = SimConfig::Default();
    const float currentCharge = robot.getBattery().getChargePercentage();

    if (percentage >= currentCharge) {
        robot.chargeBy(percentage - currentCharge);
        return;
    }

    const Vector2 originalPosition = robot.getPosition();
    const float distance = (currentCharge - percentage) / simConfig.batteryDrainPerPixel;
    const float speed = RobotConfig().motion.speed;

    robot.setTargetPosition({originalPosition.x + distance, originalPosition.y});
    robot.updateMovement(distance / speed);
    robot.setPosition(originalPosition);
}

float EstimatedNextDeliveryBatteryUse(const LogisticsMap& logisticsMap,
                                      const RobotRoutePlanner& routePlanner,
                                      Vector2 robotPosition) {
    const auto pickupDock = logisticsMap.getLagerDockPosition(logisticsMap.getPickupLagerId());
    CHECK(pickupDock.has_value());
    if (!pickupDock) {
        return 0.0f;
    }

    const std::vector<Vector2> pickupPath = routePlanner.buildPathToPickup(robotPosition);
    const std::vector<Vector2> dropoffPath = routePlanner.buildPathToDropoff(*pickupDock);

    return (routePlanner.calculatePathDistance(robotPosition, pickupPath) +
            routePlanner.calculatePathDistance(*pickupDock, dropoffPath)) *
           SimConfig::Default().batteryDrainPerPixel;
}
} // namespace

TEST_CASE("Charging starts after dropoff at or below charging threshold", "[ChargingManager]") {
    LogisticsMap logisticsMap;
    logisticsMap.init();
    const RobotRoutePlanner routePlanner(logisticsMap);
    const ChargingManager chargingManager(logisticsMap);
    const auto deliveryDock = logisticsMap.getLagerDockPosition(logisticsMap.getDeliveryLagerId());
    REQUIRE(deliveryDock.has_value());
    WorkerRobot robot(*deliveryDock, RobotConfig());

    SetRobotBatteryPercentage(robot, SimConfig::Default().lowBatteryThreshold);

    CHECK(
        chargingManager.shouldStartChargingAfterDropoff(robot, routePlanner, robot.getPosition()));
}

TEST_CASE("Charging starts when next delivery would violate minimum battery", "[ChargingManager]") {
    LogisticsMap logisticsMap;
    logisticsMap.init();
    const RobotRoutePlanner routePlanner(logisticsMap);
    const ChargingManager chargingManager(logisticsMap);
    const auto deliveryDock = logisticsMap.getLagerDockPosition(logisticsMap.getDeliveryLagerId());
    REQUIRE(deliveryDock.has_value());
    WorkerRobot robot(*deliveryDock, RobotConfig());

    const float nextDeliveryUse =
        EstimatedNextDeliveryBatteryUse(logisticsMap, routePlanner, robot.getPosition());
    SetRobotBatteryPercentage(robot, SimConfig::Default().emergencyBatteryThreshold +
                                         nextDeliveryUse - kBatteryTestMargin);

    CHECK(
        chargingManager.shouldStartChargingAfterDropoff(robot, routePlanner, robot.getPosition()));
}

TEST_CASE("Charging is skipped when battery can complete next delivery safely",
          "[ChargingManager]") {
    LogisticsMap logisticsMap;
    logisticsMap.init();
    const RobotRoutePlanner routePlanner(logisticsMap);
    const ChargingManager chargingManager(logisticsMap);
    const auto deliveryDock = logisticsMap.getLagerDockPosition(logisticsMap.getDeliveryLagerId());
    REQUIRE(deliveryDock.has_value());
    WorkerRobot robot(*deliveryDock, RobotConfig());

    const float nextDeliveryUse =
        EstimatedNextDeliveryBatteryUse(logisticsMap, routePlanner, robot.getPosition());
    SetRobotBatteryPercentage(robot, SimConfig::Default().emergencyBatteryThreshold +
                                         nextDeliveryUse + 1.0f);

    CHECK_FALSE(
        chargingManager.shouldStartChargingAfterDropoff(robot, routePlanner, robot.getPosition()));
}
