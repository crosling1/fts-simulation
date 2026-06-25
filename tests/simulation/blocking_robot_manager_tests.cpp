#include "support/test_helpers.h"

#include <catch2/catch_test_macros.hpp>

#include "simulation/BlockingRobotManager.h"

TEST_CASE("Blocking robot detection ignores inactive robots", "[BlockingRobotManager]") {
    BlockingRobotManager manager;

    manager.addBlockingRobot({
        {10.0f, 0.0f},
        5.0f,
        0.0f,
        {},
        0,
        0,
        0,
        true,
    });
    manager.addBlockingRobot({
        {2.0f, 0.0f},
        5.0f,
        0.0f,
        {},
        0,
        0,
        0,
        false,
    });

    CHECK(manager.hasActiveBlockingRobotNear({0.0f, 0.0f}, 6.0f));
    CHECK(!manager.hasActiveBlockingRobotNear({0.0f, 0.0f}, 4.0f));
}

TEST_CASE("Blocking robot outside radius is not detected", "[BlockingRobotManager]") {
    BlockingRobotManager manager;
    manager.addBlockingRobot({
        {20.0f, 0.0f},
        2.0f,
        0.0f,
        {},
        0,
        0,
        0,
        true,
    });

    CHECK(!manager.hasActiveBlockingRobotNear({0.0f, 0.0f}, 6.0f));
}

TEST_CASE("Blocking robot inside radius is detected", "[BlockingRobotManager]") {
    BlockingRobotManager manager;
    manager.addBlockingRobot({
        {5.0f, 0.0f},
        2.0f,
        0.0f,
        {},
        0,
        0,
        0,
        true,
    });

    CHECK(manager.hasActiveBlockingRobotNear({0.0f, 0.0f}, 6.0f));
}

TEST_CASE("Blocking robot moves along path", "[BlockingRobotManager]") {
    BlockingRobotManager manager;
    manager.addBlockingRobot({
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
    test::CheckVectorNear(manager.getBlockingRobots()[0].position, {5.0f, 0.0f});

    manager.update(0.5f);
    test::CheckVectorNear(manager.getBlockingRobots()[0].position, {10.0f, 0.0f});
}
