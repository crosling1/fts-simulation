#include "support/test_helpers.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "sensors/ProximitySensor.h"
#include "simulation/BlockingRobotManager.h"

TEST_CASE("Proximity sensor uses blocking robot manager", "[BlockingRobotManager]") {
    BlockingRobotManager manager;
    manager.addBlockingRobot({
        {8.0f, 0.0f},
        2.0f,
        0.0f,
        {},
        0,
        0,
        0,
    });
    ProximitySensor proximitySensor(6.0f);

    CHECK(proximitySensor.getDetectionRadius() == Catch::Approx(6.0f).margin(test::epsilon));
    CHECK(proximitySensor.hasBlockingRobotNearby({0.0f, 0.0f}, manager));
}
