#include "support/MockLogisticsMap.h"
#include "support/test_helpers.h"

#include <catch2/catch_test_macros.hpp>

#include <optional>
#include <vector>

#include "simulation/RobotRoutePlanner.h"

TEST_CASE("Robot route planner uses logistics map interface", "[RobotRoutePlanner]") {
    MockLogisticsMap map;
    const RobotRoutePlanner routePlanner(map);

    const std::vector<Vector2> path = routePlanner.buildPathToPickup({0.0f, 0.0f});

    REQUIRE_FALSE(path.empty());
    test::CheckVectorNear(path.back(), {10.0f, 0.0f});
}

TEST_CASE("Robot route planner returns empty path when pickup dock is missing",
          "[RobotRoutePlanner]") {
    MockLogisticsMap map;
    map.setPickupDockPosition(std::nullopt);
    const RobotRoutePlanner routePlanner(map);

    CHECK(routePlanner.buildPathToPickup({0.0f, 0.0f}).empty());
}
