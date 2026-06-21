#include "support/test_helpers.h"
#include "support/test_runner.h"
#include "support/test_suites.h"

#include "robots/WorkerRobot.h"

#include <string>

namespace {
void TestRobotMovesToTarget(void) {
    WorkerRobot robot({0.0f, 0.0f}, {10.0f, 90.0f, 8.0f});

    robot.setTargetPosition({10.0f, 0.0f});
    test::Expect(robot.getState() == Robot::State::Moving,
                 "robot should start moving to a new target");

    robot.update(0.5f);

    Vector2 position = {0.0f, 0.0f};
    robot.getPosition(position);
    test::ExpectVectorNear(position, {5.0f, 0.0f}, "robot should move by speed * deltaTime");
    test::Expect(robot.getState() == Robot::State::Moving,
                 "robot should still be moving before target");

    robot.update(0.5f);
    robot.getPosition(position);
    test::ExpectVectorNear(position, {10.0f, 0.0f}, "robot should stop exactly at the target");
    test::Expect(robot.getState() == Robot::State::Arrived, "robot should arrive at target");
}

void TestRobotPiControllerLimitsSpeedNearTarget(void) {
    WorkerRobot robot({0.0f, 0.0f}, {100.0f, 90.0f, 8.0f, 0.5f, 0.1f, 100.0f});

    robot.setTargetPosition({10.0f, 0.0f});
    robot.update(1.0f);

    Vector2 position = {0.0f, 0.0f};
    robot.getPosition(position);
    test::ExpectVectorNear(position, {6.0f, 0.0f},
                           "PI controller should command less than max speed near target");
    test::Expect(robot.getState() == Robot::State::Moving,
                 "PI controlled robot should continue moving before target threshold");
}

void TestRobotKeepsCarryingStateWhenArriving(void) {
    WorkerRobot robot({0.0f, 0.0f}, {10.0f, 90.0f, 8.0f});

    robot.setState(Robot::State::CarryingItem);
    robot.setTargetPosition({10.0f, 0.0f});
    robot.update(1.0f);

    Vector2 position = {0.0f, 0.0f};
    robot.getPosition(position);
    test::ExpectVectorNear(position, {10.0f, 0.0f}, "carrying robot should reach target");
    test::Expect(robot.getState() == Robot::State::CarryingItem,
                 "carrying robot should not switch to Arrived during route");
}

void TestRobotRotatesTowardTarget(void) {
    WorkerRobot robot({0.0f, 0.0f}, {10.0f, 90.0f, 8.0f});

    robot.setTargetPosition({0.0f, -10.0f});
    robot.update(0.5f);

    float rotation = 0.0f;
    robot.getRotation(rotation);
    test::Expect(test::AlmostEqual(rotation, -45.0f),
                 "robot rotation should be limited by rotation speed");

    robot.update(0.5f);
    robot.getRotation(rotation);
    test::Expect(test::AlmostEqual(rotation, -90.0f),
                 "robot should finish rotating toward the target");
}

void TestRobotOwnsBattery(void) {
    WorkerRobot robot({0.0f, 0.0f}, {10.0f, 90.0f, 8.0f});

    test::Expect(test::AlmostEqual(robot.getBattery().getChargePercentage(), 100.0f),
                 "robot battery should start full");

    robot.getBattery().drain(91.0f);
    test::Expect(test::AlmostEqual(robot.getBattery().getChargePercentage(), 9.0f),
                 "robot battery should drain by percentage");
    test::Expect(robot.getBattery().isLow(10.0f), "robot battery should report low charge");

    robot.getBattery().charge(200.0f);
    test::Expect(test::AlmostEqual(robot.getBattery().getChargePercentage(), 100.0f),
                 "robot battery should clamp charge to full");
    test::Expect(robot.getBattery().isFull(), "robot battery should report full charge");

    robot.getBattery().drain(100.0f);
    test::Expect(robot.getBattery().isEmpty(), "robot battery should report empty charge");
}

void TestRobotDrainsBatteryByDistanceMoved(void) {
    WorkerRobot robot({0.0f, 0.0f}, {100.0f, 90.0f, 8.0f});

    robot.setTargetPosition({100.0f, 0.0f});
    robot.update(1.0f);

    test::Expect(test::AlmostEqual(robot.getBattery().getChargePercentage(), 99.0f),
                 "robot battery should drain 1 percent per 100 pixels moved");

    robot.getBattery().charge(1.0f);
    test::Expect(test::AlmostEqual(robot.getBattery().getChargePercentage(), 100.0f),
                 "robot battery should recharge by percentage");
}

void TestRobotStopsWhenBatteryIsEmpty(void) {
    WorkerRobot robot({0.0f, 0.0f}, {10.0f, 90.0f, 8.0f});

    robot.getBattery().setChargePercentage(0.0f);
    robot.setTargetPosition({10.0f, 0.0f});

    test::Expect(robot.getState() == Robot::State::BatteryDepleted,
                 "empty battery robot should enter battery depleted state");

    robot.update(1.0f);

    Vector2 position = {0.0f, 0.0f};
    robot.getPosition(position);
    test::ExpectVectorNear(position, {0.0f, 0.0f}, "empty battery robot should not move");

    robot.getBattery().charge(50.0f);
    robot.setTargetPosition({10.0f, 0.0f});
    robot.update(1.0f);
    robot.getPosition(position);
    test::ExpectVectorNear(position, {10.0f, 0.0f}, "charged robot should move again");
}

const test::TestCase robotTests[] = {
    {"Robot moves to target", TestRobotMovesToTarget},
    {"Robot PI controller limits speed near target", TestRobotPiControllerLimitsSpeedNearTarget},
    {"Robot keeps carrying state when arriving", TestRobotKeepsCarryingStateWhenArriving},
    {"Robot rotates toward target", TestRobotRotatesTowardTarget},
    {"Robot owns battery", TestRobotOwnsBattery},
    {"Robot drains battery by distance moved", TestRobotDrainsBatteryByDistanceMoved},
    {"Robot stops when battery is empty", TestRobotStopsWhenBatteryIsEmpty},
};
} // namespace

void RunRobotTests(void) {
    test::RunTestCases(robotTests);
}

bool RunRobotTestByName(const std::string& name) {
    return test::RunTestCaseByName(robotTests, name);
}
