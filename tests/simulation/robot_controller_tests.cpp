#include "support/test_helpers.h"

#include <catch2/catch_test_macros.hpp>

#include "robots/RobotState.h"
#include "simulation/BlockingRobotManager.h"
#include "simulation/InputState.h"
#include "simulation/RobotController.h"
#include "simulation/SimConfig.h"
#include "simulation/map.h"

namespace {
constexpr float kControllerUpdateTimestep = 0.1f;

const char* RobotStateName(RobotState state) {
    switch (state) {
    case RobotState::Idle:
        return "Idle";
    case RobotState::Moving:
        return "Moving";
    case RobotState::PickingUp:
        return "PickingUp";
    case RobotState::CarryingItem:
        return "CarryingItem";
    case RobotState::DroppingOff:
        return "DroppingOff";
    case RobotState::Arrived:
        return "Arrived";
    case RobotState::BatteryDepleted:
        return "BatteryDepleted";
    case RobotState::Charging:
        return "Charging";
    }

    return "Unknown";
}

RobotStatusSnapshot RequireSnapshot(const RobotController& controller) {
    const std::optional<RobotStatusSnapshot> snapshot = controller.statusSnapshot();
    REQUIRE(snapshot.has_value());
    return snapshot.value_or(RobotStatusSnapshot{RobotState::Idle, 0.0f, false});
}

bool AdvanceUntilState(RobotController& controller, RobotState expectedState,
                       float maxSeconds = 30.0f) {
    InputState inputState;
    float elapsedSeconds = 0.0f;
    RobotStatusSnapshot lastSnapshot = RequireSnapshot(controller);

    while (elapsedSeconds <= maxSeconds) {
        controller.update(kControllerUpdateTimestep, inputState);
        lastSnapshot = RequireSnapshot(controller);
        if (lastSnapshot.state == expectedState) {
            return true;
        }
        elapsedSeconds += kControllerUpdateTimestep;
    }

    INFO("Expected state: " << RobotStateName(expectedState));
    INFO("Last observed state: " << RobotStateName(lastSnapshot.state));
    INFO("Battery percentage: " << lastSnapshot.batteryPercentage);
    INFO("Elapsed simulated time: " << elapsedSeconds);

    return false;
}
} // namespace

TEST_CASE("Robot controller initializes and publishes robot status", "[RobotController]") {
    LogisticsMap logisticsMap;
    logisticsMap.init();
    const BlockingRobotManager blockingRobotManager;
    RobotController controller(logisticsMap, blockingRobotManager);

    CHECK_FALSE(controller.statusSnapshot().has_value());

    controller.initialize();

    const RobotStatusSnapshot snapshot = RequireSnapshot(controller);
    CHECK(snapshot.state == RobotState::Moving);
    CHECK(snapshot.batteryPercentage == Catch::Approx(100.0f).margin(test::epsilon));
    CHECK_FALSE(snapshot.emergencyStopActive);
}

TEST_CASE("Robot controller advances pickup and dropoff workflow", "[RobotController]") {
    LogisticsMap logisticsMap;
    logisticsMap.init();
    const BlockingRobotManager blockingRobotManager;
    RobotController controller(logisticsMap, blockingRobotManager);

    controller.initialize();

    REQUIRE(AdvanceUntilState(controller, RobotState::PickingUp));

    controller.update(SimConfig::Default().pickupDurationSeconds, InputState{});

    CHECK(RequireSnapshot(controller).state == RobotState::CarryingItem);
    CHECK(AdvanceUntilState(controller, RobotState::DroppingOff));
}
