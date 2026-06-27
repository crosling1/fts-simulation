#include "support/test_helpers.h"

#include <catch2/catch_test_macros.hpp>

#include "simulation/RobotTaskFlow.h"
#include "simulation/SimConstants.h"

TEST_CASE("Robot task flow starts routed to pickup and resets timers", "[RobotTaskFlow]") {
    RobotTaskFlow taskFlow;

    CHECK(taskFlow.isRoutingToPickup());
    CHECK_FALSE(taskFlow.isPickingUp());

    taskFlow.startPickingUp();
    REQUIRE_FALSE(taskFlow.updatePickup(SimConstants::Task::kPickupDurationSeconds - 0.1f));

    taskFlow.reset();

    CHECK(taskFlow.isRoutingToPickup());
    CHECK_FALSE(taskFlow.updatePickup(0.1f));
}

TEST_CASE("Robot task flow exposes explicit phase transitions", "[RobotTaskFlow]") {
    RobotTaskFlow taskFlow;

    taskFlow.startPickingUp();
    CHECK(taskFlow.isPickingUp());

    taskFlow.startTripToDropoff();
    CHECK(taskFlow.isRoutingToDropoff());

    taskFlow.startDroppingOff();
    CHECK(taskFlow.isDroppingOff());

    taskFlow.startTripToCharging();
    CHECK(taskFlow.isRoutingToCharging());

    taskFlow.startCharging();
    CHECK(taskFlow.isCharging());

    taskFlow.startTripToPickup();
    CHECK(taskFlow.isRoutingToPickup());
}

TEST_CASE("Robot task flow completes pickup and dropoff after configured durations",
          "[RobotTaskFlow]") {
    RobotTaskFlow taskFlow;

    taskFlow.startPickingUp();
    CHECK_FALSE(taskFlow.updatePickup(SimConstants::Task::kPickupDurationSeconds - 0.01f));
    CHECK(taskFlow.updatePickup(0.01f));

    taskFlow.startDroppingOff();
    CHECK_FALSE(taskFlow.updateDropoff(SimConstants::Task::kDropoffDurationSeconds - 0.01f));
    CHECK(taskFlow.updateDropoff(0.01f));
}
