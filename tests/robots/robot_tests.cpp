#include "support/test_helpers.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "robots/WorkerRobot.h"

namespace {
Robot::Config RobotConfig(float speed, float rotationSpeed, float size,
                          float proportionalGain = 0.0f, float integralGain = 0.0f,
                          float maxIntegralError = 1000.0f) {
    return {
        {speed, rotationSpeed, size},
        {proportionalGain, integralGain, maxIntegralError},
    };
}
} // namespace

TEST_CASE("Robot moves to target", "[Robot]") {
    WorkerRobot robot(Vector2{0.0f, 0.0f}, RobotConfig(10.0f, 90.0f, 8.0f));

    robot.setTargetPosition({10.0f, 0.0f});
    REQUIRE(robot.getState() == Robot::State::Moving);

    robot.updateMovement(0.5f);

    const Vector2 position = robot.getPosition();
    test::CheckVectorNear(position, {5.0f, 0.0f});
    CHECK(robot.getState() == Robot::State::Moving);

    robot.updateMovement(0.5f);
    const Vector2 finalPosition = robot.getPosition();
    test::CheckVectorNear(finalPosition, {10.0f, 0.0f});
    CHECK(robot.getState() == Robot::State::Arrived);
}

TEST_CASE("Robot PI controller limits speed near target", "[Robot]") {
    WorkerRobot robot(Vector2{0.0f, 0.0f}, RobotConfig(100.0f, 90.0f, 8.0f, 0.5f, 0.1f, 100.0f));

    robot.setTargetPosition({10.0f, 0.0f});
    robot.updateMovement(1.0f);

    const Vector2 position = robot.getPosition();
    test::CheckVectorNear(position, {6.0f, 0.0f});
    CHECK(robot.getState() == Robot::State::Moving);
}

TEST_CASE("Robot keeps carrying state when arriving", "[Robot]") {
    WorkerRobot robot(Vector2{0.0f, 0.0f}, RobotConfig(10.0f, 90.0f, 8.0f));

    robot.setState(Robot::State::CarryingItem);
    robot.setTargetPosition({10.0f, 0.0f});
    robot.updateMovement(1.0f);

    const Vector2 position = robot.getPosition();
    test::CheckVectorNear(position, {10.0f, 0.0f});
    CHECK(robot.getState() == Robot::State::CarryingItem);
}

TEST_CASE("Robot rotates toward target", "[Robot]") {
    WorkerRobot robot(Vector2{0.0f, 0.0f}, RobotConfig(10.0f, 90.0f, 8.0f));

    robot.setTargetPosition({0.0f, -10.0f});
    robot.updateMovement(0.5f);

    CHECK(robot.angle() == Catch::Approx(-45.0).margin(test::epsilon));

    robot.updateMovement(0.5f);
    CHECK(robot.angle() == Catch::Approx(-90.0).margin(test::epsilon));
}

TEST_CASE("Battery clamps charge and reports state", "[Battery]") {
    WorkerRobot robot(Vector2{0.0f, 0.0f}, RobotConfig(10.0f, 90.0f, 8.0f));

    REQUIRE(robot.getBattery().getChargePercentage() ==
            Catch::Approx(100.0f).margin(test::epsilon));

    robot.getBattery().drain(91.0f);
    CHECK(robot.getBattery().getChargePercentage() == Catch::Approx(9.0f).margin(test::epsilon));
    CHECK(robot.getBattery().isLow(10.0f));

    robot.getBattery().charge(200.0f);
    CHECK(robot.getBattery().getChargePercentage() == Catch::Approx(100.0f).margin(test::epsilon));
    CHECK(robot.getBattery().isFull());

    robot.getBattery().drain(100.0f);
    CHECK(robot.getBattery().isEmpty());
}

TEST_CASE("Robot drains battery by distance moved", "[Robot][Battery]") {
    WorkerRobot robot(Vector2{0.0f, 0.0f}, RobotConfig(100.0f, 90.0f, 8.0f));

    robot.setTargetPosition({100.0f, 0.0f});
    robot.updateMovement(1.0f);

    CHECK(robot.getBattery().getChargePercentage() == Catch::Approx(99.0f).margin(test::epsilon));

    robot.getBattery().charge(1.0f);
    CHECK(robot.getBattery().getChargePercentage() == Catch::Approx(100.0f).margin(test::epsilon));
}

TEST_CASE("Robot stops when battery is empty", "[Robot][Battery]") {
    WorkerRobot robot(Vector2{0.0f, 0.0f}, RobotConfig(10.0f, 90.0f, 8.0f));

    robot.getBattery().setChargePercentage(0.0f);
    robot.setTargetPosition({10.0f, 0.0f});

    REQUIRE(robot.getState() == Robot::State::BatteryDepleted);

    robot.updateMovement(1.0f);

    const Vector2 position = robot.getPosition();
    test::CheckVectorNear(position, {0.0f, 0.0f});

    robot.getBattery().charge(50.0f);
    robot.setTargetPosition({10.0f, 0.0f});
    robot.updateMovement(1.0f);
    const Vector2 chargedPosition = robot.getPosition();
    test::CheckVectorNear(chargedPosition, {10.0f, 0.0f});
}
