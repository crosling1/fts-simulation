#include <catch2/catch_test_macros.hpp>

#include "robots/WorkerRobot.h"
#include "simulation/BlockingRobotManager.h"
#include "simulation/EmergencyStopController.h"
#include "simulation/InputState.h"

namespace {
Robot::Config RobotConfig() {
    return {
        {10.0f, 90.0f, 8.0f},
        {0.0f, 0.0f, 1000.0f},
    };
}
} // namespace

TEST_CASE("Emergency stop restores the saved robot state on reset", "[EmergencyStopController]") {
    const BlockingRobotManager blockingRobotManager;
    EmergencyStopController controller(blockingRobotManager);
    WorkerRobot robot(Vector2{0.0f, 0.0f}, RobotConfig());

    robot.setState(Robot::State::Moving);

    controller.updateEmergencyStop(InputState{true, false}, robot);

    REQUIRE(controller.isEmergencyStopActive());
    CHECK(robot.getState() == Robot::State::Idle);

    controller.updateEmergencyStop(InputState{false, true}, robot);

    CHECK_FALSE(controller.isEmergencyStopActive());
    CHECK(robot.getState() == Robot::State::Moving);
}

TEST_CASE("Emergency stop resets battery depleted state to idle", "[EmergencyStopController]") {
    const BlockingRobotManager blockingRobotManager;
    EmergencyStopController controller(blockingRobotManager);
    WorkerRobot robot(Vector2{0.0f, 0.0f}, RobotConfig());

    robot.setState(Robot::State::BatteryDepleted);

    controller.updateEmergencyStop(InputState{true, false}, robot);
    controller.updateEmergencyStop(InputState{false, true}, robot);

    CHECK_FALSE(controller.isEmergencyStopActive());
    CHECK(robot.getState() == Robot::State::Idle);
}
