#include "support/test_helpers.h"

#include <catch2/catch_test_macros.hpp>

#include "robots/Battery.h"
#include "robots/WorkerRobot.h"
#include "simulation/ChargingManager.h"
#include "simulation/RobotRoutePlanner.h"
#include "simulation/SimConfig.h"
#include "simulation/map.h"

#include <optional>

namespace {
constexpr float kBatteryTestMargin = 0.01f;

Robot::Config RobotConfig() {
    return {
        {10.0f, 90.0f, 8.0f},
        {0.0f, 0.0f, 1000.0f},
    };
}

void SetRobotBatteryPercentage(WorkerRobot& robot, float percentage) {
    const_cast<Battery&>(robot.getBattery()).setChargePercentage(percentage);
}

float EstimatedNextDeliveryDistance(const LogisticsMap& logisticsMap,
                                    const RobotRoutePlanner& routePlanner, Vec2 robotPosition) {
    const Vec2 fallbackPosition{0.0F, 0.0F};
    const auto pickupDock = logisticsMap.getLagerDockPosition(logisticsMap.getPickupLagerId());
    REQUIRE(pickupDock.has_value());

    const Vec2 pickupDockPosition = pickupDock.value_or(fallbackPosition);
    const std::vector<Vec2> pickupPath = routePlanner.buildPathToPickup(robotPosition);
    const std::vector<Vec2> dropoffPath = routePlanner.buildPathToDropoff(pickupDockPosition);

    return routePlanner.calculatePathDistance(robotPosition, pickupPath) +
           routePlanner.calculatePathDistance(pickupDockPosition, dropoffPath);
}
} // namespace

TEST_CASE("Charging starts after dropoff at or below charging threshold", "[ChargingManager]") {
    LogisticsMap logisticsMap;
    logisticsMap.init();
    const RobotRoutePlanner routePlanner(logisticsMap);
    const ChargingManager chargingManager;
    const auto deliveryDock = logisticsMap.getLagerDockPosition(logisticsMap.getDeliveryLagerId());
    REQUIRE(deliveryDock.has_value());
    const Vec2 deliveryDockPosition = deliveryDock.value_or(Vec2{});
    WorkerRobot robot(deliveryDockPosition, RobotConfig());

    SetRobotBatteryPercentage(robot, SimConfig::Default().lowBatteryThreshold);

    CHECK(chargingManager.shouldStartChargingAfterDropoff(robot, std::nullopt));
}

TEST_CASE("Charging starts when next delivery would violate minimum battery", "[ChargingManager]") {
    LogisticsMap logisticsMap;
    logisticsMap.init();
    const RobotRoutePlanner routePlanner(logisticsMap);
    const ChargingManager chargingManager;
    const auto deliveryDock = logisticsMap.getLagerDockPosition(logisticsMap.getDeliveryLagerId());
    REQUIRE(deliveryDock.has_value());
    const Vec2 deliveryDockPosition = deliveryDock.value_or(Vec2{});
    WorkerRobot robot(deliveryDockPosition, RobotConfig());

    const float nextDeliveryDistance =
        EstimatedNextDeliveryDistance(logisticsMap, routePlanner, robot.getPosition());
    const float nextDeliveryUse = nextDeliveryDistance * SimConfig::Default().batteryDrainPerPixel;
    SetRobotBatteryPercentage(robot, SimConfig::Default().emergencyBatteryThreshold +
                                         nextDeliveryUse - kBatteryTestMargin);

    CHECK(chargingManager.shouldStartChargingAfterDropoff(robot, nextDeliveryDistance));
}

TEST_CASE("Charging is skipped when battery can complete next delivery safely",
          "[ChargingManager]") {
    LogisticsMap logisticsMap;
    logisticsMap.init();
    const RobotRoutePlanner routePlanner(logisticsMap);
    const ChargingManager chargingManager;
    const auto deliveryDock = logisticsMap.getLagerDockPosition(logisticsMap.getDeliveryLagerId());
    REQUIRE(deliveryDock.has_value());
    const Vec2 deliveryDockPosition = deliveryDock.value_or(Vec2{});
    WorkerRobot robot(deliveryDockPosition, RobotConfig());

    const float nextDeliveryDistance =
        EstimatedNextDeliveryDistance(logisticsMap, routePlanner, robot.getPosition());
    const float nextDeliveryUse = nextDeliveryDistance * SimConfig::Default().batteryDrainPerPixel;
    SetRobotBatteryPercentage(robot, SimConfig::Default().emergencyBatteryThreshold +
                                         nextDeliveryUse + 1.0f);

    CHECK(!chargingManager.shouldStartChargingAfterDropoff(robot, nextDeliveryDistance));
}
