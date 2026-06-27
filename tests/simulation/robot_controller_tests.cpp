#include "support/test_helpers.h"

#include <catch2/catch_test_macros.hpp>

#include "robots/RobotState.h"
#include "simulation/BlockingRobotManager.h"
#include "simulation/InputState.h"
#include "simulation/RobotController.h"
#include "simulation/SimConfig.h"
#include "simulation/map.h"

namespace {
RobotStatusSnapshot RequireSnapshot(const RobotController& controller) {
    const std::optional<RobotStatusSnapshot> snapshot = controller.statusSnapshot();
    REQUIRE(snapshot.has_value());
    return snapshot.value_or(RobotStatusSnapshot{RobotState::Idle, 0.0f, false});
}

bool AdvanceUntilState(RobotController& controller, RobotState expectedState,
                       float maxSeconds = 30.0f) {
    InputState inputState;
    float elapsedSeconds = 0.0f;
    while (elapsedSeconds <= maxSeconds) {
        controller.update(0.1f, inputState);
        if (RequireSnapshot(controller).state == expectedState) {
            return true;
        }
        elapsedSeconds += 0.1f;
    }

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
