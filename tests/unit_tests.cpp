#include "robots/WorkerRobot.h"
#include "sensors/LidarSensor.h"
#include "simulation/ObstacleManager.h"
#include "simulation/map.h"
#include "simulation/navigation.h"

#include <cmath>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
constexpr float epsilon = 0.001f;

bool AlmostEqual(float left, float right) {
    return std::fabs(left - right) <= epsilon;
}

void Expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void ExpectVectorNear(Vector2 actual, Vector2 expected, const std::string& message) {
    if (!AlmostEqual(actual.x, expected.x) || !AlmostEqual(actual.y, expected.y)) {
        throw std::runtime_error(message + " expected (" + std::to_string(expected.x) + ", " +
                                 std::to_string(expected.y) + ") got (" + std::to_string(actual.x) +
                                 ", " + std::to_string(actual.y) + ")");
    }
}

void TestRobotMovesToTarget(void) {
    WorkerRobot robot({0.0f, 0.0f}, {10.0f, 90.0f, 8.0f});

    robot.setTargetPosition({10.0f, 0.0f});
    Expect(robot.getState() == Robot::State::Moving, "robot should start moving to a new target");

    robot.update(0.5f);

    Vector2 position = {0.0f, 0.0f};
    robot.getPosition(position);
    ExpectVectorNear(position, {5.0f, 0.0f}, "robot should move by speed * deltaTime");
    Expect(robot.getState() == Robot::State::Moving, "robot should still be moving before target");

    robot.update(0.5f);
    robot.getPosition(position);
    ExpectVectorNear(position, {10.0f, 0.0f}, "robot should stop exactly at the target");
    Expect(robot.getState() == Robot::State::Arrived, "robot should arrive at target");
}

void TestRobotKeepsCarryingStateWhenArriving(void) {
    WorkerRobot robot({0.0f, 0.0f}, {10.0f, 90.0f, 8.0f});

    robot.setState(Robot::State::CarryingItem);
    robot.setTargetPosition({10.0f, 0.0f});
    robot.update(1.0f);

    Vector2 position = {0.0f, 0.0f};
    robot.getPosition(position);
    ExpectVectorNear(position, {10.0f, 0.0f}, "carrying robot should reach target");
    Expect(robot.getState() == Robot::State::CarryingItem,
           "carrying robot should not switch to Arrived during route");
}

void TestRobotRotatesTowardTarget(void) {
    WorkerRobot robot({0.0f, 0.0f}, {10.0f, 90.0f, 8.0f});

    robot.setTargetPosition({0.0f, -10.0f});
    robot.update(0.5f);

    float rotation = 0.0f;
    robot.getRotation(rotation);
    Expect(AlmostEqual(rotation, -45.0f), "robot rotation should be limited by rotation speed");

    robot.update(0.5f);
    robot.getRotation(rotation);
    Expect(AlmostEqual(rotation, -90.0f), "robot should finish rotating toward the target");
}

void TestRobotOwnsBattery(void) {
    WorkerRobot robot({0.0f, 0.0f}, {10.0f, 90.0f, 8.0f});

    Expect(AlmostEqual(robot.getBattery().getChargePercentage(), 100.0f),
           "robot battery should start full");

    robot.getBattery().drain(91.0f);
    Expect(AlmostEqual(robot.getBattery().getChargePercentage(), 9.0f),
           "robot battery should drain by percentage");
    Expect(robot.getBattery().isLow(10.0f), "robot battery should report low charge");

    robot.getBattery().charge(200.0f);
    Expect(AlmostEqual(robot.getBattery().getChargePercentage(), 100.0f),
           "robot battery should clamp charge to full");
    Expect(robot.getBattery().isFull(), "robot battery should report full charge");

    robot.getBattery().drain(100.0f);
    Expect(robot.getBattery().isEmpty(), "robot battery should report empty charge");
}

void TestRobotDrainsBatteryByDistanceMoved(void) {
    WorkerRobot robot({0.0f, 0.0f}, {100.0f, 90.0f, 8.0f});

    robot.setTargetPosition({100.0f, 0.0f});
    robot.update(1.0f);

    Expect(AlmostEqual(robot.getBattery().getChargePercentage(), 99.0f),
           "robot battery should drain 1 percent per 100 pixels moved");

    robot.getBattery().charge(1.0f);
    Expect(AlmostEqual(robot.getBattery().getChargePercentage(), 100.0f),
           "robot battery should recharge by percentage");
}

void TestRobotStopsWhenBatteryIsEmpty(void) {
    WorkerRobot robot({0.0f, 0.0f}, {10.0f, 90.0f, 8.0f});

    robot.getBattery().setChargePercentage(0.0f);
    robot.setTargetPosition({10.0f, 0.0f});

    Expect(robot.getState() == Robot::State::BatteryDepleted,
           "empty battery robot should enter battery depleted state");

    robot.update(1.0f);

    Vector2 position = {0.0f, 0.0f};
    robot.getPosition(position);
    ExpectVectorNear(position, {0.0f, 0.0f}, "empty battery robot should not move");

    robot.getBattery().charge(50.0f);
    robot.setTargetPosition({10.0f, 0.0f});
    robot.update(1.0f);
    robot.getPosition(position);
    ExpectVectorNear(position, {10.0f, 0.0f}, "charged robot should move again");
}

void TestObstacleDetectionIgnoresInactiveObstacles(void) {
    ObstacleManager manager;

    manager.addObstacle({
        {10.0f, 0.0f},
        5.0f,
        0.0f,
        {},
        0,
        0,
        0,
        true,
    });
    manager.addObstacle({
        {2.0f, 0.0f},
        5.0f,
        0.0f,
        {},
        0,
        0,
        0,
        false,
    });

    Expect(manager.hasActiveObstacleNear({0.0f, 0.0f}, 6.0f),
           "active obstacle should be detected when circles overlap");
    Expect(!manager.hasActiveObstacleNear({0.0f, 0.0f}, 4.0f),
           "active obstacle should not be detected outside combined radii");
}

void TestObstacleMovesAlongPath(void) {
    ObstacleManager manager;
    manager.addObstacle({
        {0.0f, 0.0f},
        5.0f,
        10.0f,
        {{0.0f, 0.0f}, {10.0f, 0.0f}},
        0,
        1,
        0,
        true,
    });

    manager.update(0.5f);
    ExpectVectorNear(manager.getObstacles()[0].position, {5.0f, 0.0f},
                     "obstacle should move along path");

    manager.update(0.5f);
    ExpectVectorNear(manager.getObstacles()[0].position, {10.0f, 0.0f},
                     "obstacle should snap to target when step reaches it");
}

void TestLidarUsesObstacleManager(void) {
    ObstacleManager manager;
    manager.addObstacle({
        {8.0f, 0.0f},
        2.0f,
        0.0f,
        {},
        0,
        0,
        0,
        true,
    });
    LidarSensor lidar(6.0f);

    Expect(AlmostEqual(lidar.getDetectionRadius(), 6.0f), "lidar should expose detection radius");
    Expect(lidar.hasObstacleNearby({0.0f, 0.0f}, manager),
           "lidar should report nearby active obstacles");
}

void TestMapRoadQueries(void) {
    InitMap();

    const Vector2 start = GetRobotStartPosition();
    const Vector2 offRoad = {20.0f, 20.0f};
    const Vector2 clamped = ClampPositionToMapRoad(offRoad);

    Expect(IsMapRoadPosition(start), "robot start should be on a road");
    Expect(IsMapRoadPosition(GetChargingStationDockPosition()),
           "charging station dock should be on a road");
    Expect(!IsMapRoadPosition(offRoad), "off-road point should not be on a road");
    Expect(IsMapRoadPosition(clamped), "clamped off-road point should land on a road");
    ExpectVectorNear(GetLagerDockPosition(static_cast<LagerId>(LAGER_COUNT)), {0.0f, 0.0f},
                     "invalid lager id should return origin");
}

void TestNavigationFindsWarehouseRoutes(void) {
    InitMap();

    const std::vector<Vector2> pickupPath =
        FindNavigationPath(GetRobotStartPosition(), GetLagerDockPosition(GetMapPickupLagerId()));
    const std::vector<Vector2> dropoffPath =
        FindNavigationPath(GetLagerDockPosition(GetMapPickupLagerId()),
                           GetLagerDockPosition(GetMapDeliveryLagerId()));
    const std::vector<Vector2> chargingPath =
        FindNavigationPath(GetRobotStartPosition(), GetChargingStationDockPosition());
    const std::vector<Vector2> dropoffToChargingPath =
        FindNavigationPath(GetLagerDockPosition(GetMapDeliveryLagerId()),
                           GetChargingStationDockPosition());

    Expect(!pickupPath.empty(), "navigation should find a path to pickup lager");
    Expect(!dropoffPath.empty(), "navigation should find a path from pickup to dropoff lager");
    Expect(!chargingPath.empty(), "navigation should find a path to charging station");
    Expect(!dropoffToChargingPath.empty(),
           "navigation should find a path from dropoff to charging station");
    ExpectVectorNear(pickupPath.back(), GetLagerDockPosition(GetMapPickupLagerId()),
                     "pickup path should end at pickup dock");
    ExpectVectorNear(dropoffPath.back(), GetLagerDockPosition(GetMapDeliveryLagerId()),
                     "dropoff path should end at dropoff dock");
    ExpectVectorNear(chargingPath.back(), GetChargingStationDockPosition(),
                     "charging path should end at charging station dock");
    ExpectVectorNear(dropoffToChargingPath.back(), GetChargingStationDockPosition(),
                     "dropoff to charging path should end at charging station dock");
    Expect(dropoffToChargingPath.size() <= 3,
           "dropoff to charging path should use the direct right-side road");

    for (Vector2 waypoint : pickupPath) {
        Expect(IsMapRoadPosition(waypoint), "pickup path waypoint should be on a road");
    }

    for (Vector2 waypoint : dropoffPath) {
        Expect(IsMapRoadPosition(waypoint), "dropoff path waypoint should be on a road");
    }

    for (Vector2 waypoint : chargingPath) {
        Expect(IsMapRoadPosition(waypoint), "charging path waypoint should be on a road");
    }

    for (Vector2 waypoint : dropoffToChargingPath) {
        Expect(IsMapRoadPosition(waypoint),
               "dropoff to charging path waypoint should be on a road");
    }
}

void RunTest(const std::string& name, void (*test)(void)) {
    test();
    std::cout << "[PASS] " << name << '\n';
}
} // namespace

int main(void) {
    try {
        RunTest("Robot moves to target", TestRobotMovesToTarget);
        RunTest("Robot keeps carrying state when arriving",
                TestRobotKeepsCarryingStateWhenArriving);
        RunTest("Robot rotates toward target", TestRobotRotatesTowardTarget);
        RunTest("Robot owns battery", TestRobotOwnsBattery);
        RunTest("Robot drains battery by distance moved", TestRobotDrainsBatteryByDistanceMoved);
        RunTest("Robot stops when battery is empty", TestRobotStopsWhenBatteryIsEmpty);
        RunTest("Obstacle detection ignores inactive obstacles",
                TestObstacleDetectionIgnoresInactiveObstacles);
        RunTest("Obstacle moves along path", TestObstacleMovesAlongPath);
        RunTest("Lidar uses obstacle manager", TestLidarUsesObstacleManager);
        RunTest("Map road queries", TestMapRoadQueries);
        RunTest("Navigation finds warehouse routes", TestNavigationFindsWarehouseRoutes);
    } catch (const std::exception& error) {
        std::cerr << "[FAIL] " << error.what() << '\n';
        return 1;
    }

    return 0;
}
