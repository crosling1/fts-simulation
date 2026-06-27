#include "support/test_helpers.h"

#include <catch2/catch_test_macros.hpp>

#include "simulation/map.h"
#include "simulation/navigation.h"

#include <vector>

TEST_CASE("Navigation finds warehouse routes", "[Navigation]") {
    LogisticsMap logisticsMap;
    logisticsMap.init();

    const auto pickupDock = logisticsMap.getLagerDockPosition(logisticsMap.getPickupLagerId());
    const auto deliveryDock = logisticsMap.getLagerDockPosition(logisticsMap.getDeliveryLagerId());
    const auto l6Dock = logisticsMap.getLagerDockPosition(LagerId::L6);

    REQUIRE(pickupDock.has_value());
    REQUIRE(deliveryDock.has_value());
    REQUIRE(l6Dock.has_value());

    const Vector2 pickupDockPosition = pickupDock.value();
    const Vector2 deliveryDockPosition = deliveryDock.value();
    const Vector2 l6DockPosition = l6Dock.value();
    const std::vector<Vector2> pickupPath =
        FindNavigationPath(logisticsMap, logisticsMap.getRobotStartPosition(), pickupDockPosition);
    const std::vector<Vector2> dropoffPath =
        FindNavigationPath(logisticsMap, pickupDockPosition, deliveryDockPosition);
    const std::vector<Vector2> chargingPath =
        FindNavigationPath(logisticsMap, logisticsMap.getRobotStartPosition(),
                           logisticsMap.getChargingStationDockPosition());
    const std::vector<Vector2> dropoffToChargingPath = FindNavigationPath(
        logisticsMap, deliveryDockPosition, logisticsMap.getChargingStationDockPosition());
    const std::vector<Vector2> l6Path =
        FindNavigationPath(logisticsMap, logisticsMap.getRobotStartPosition(), l6DockPosition);
    const Vector2 l6EntryWaypoint = {545.0f, 450.0f};

    REQUIRE(!pickupPath.empty());
    REQUIRE(!dropoffPath.empty());
    REQUIRE(!chargingPath.empty());
    REQUIRE(!dropoffToChargingPath.empty());
    REQUIRE(!l6Path.empty());

    test::CheckVectorNear(pickupPath.back(), pickupDockPosition);
    test::CheckVectorNear(dropoffPath.back(), deliveryDockPosition);
    test::CheckVectorNear(chargingPath.back(), logisticsMap.getChargingStationDockPosition());
    test::CheckVectorNear(dropoffToChargingPath.back(),
                          logisticsMap.getChargingStationDockPosition());
    CHECK(dropoffToChargingPath.size() <= 3);
    CHECK(!test::PathContainsPoint(pickupPath, l6EntryWaypoint));
    CHECK(test::PathContainsPoint(l6Path, l6EntryWaypoint));

    for (Vector2 waypoint : pickupPath) {
        CAPTURE(waypoint.x, waypoint.y);
        CHECK(logisticsMap.isRoadPosition(waypoint));
    }

    for (Vector2 waypoint : dropoffPath) {
        CAPTURE(waypoint.x, waypoint.y);
        CHECK(logisticsMap.isRoadPosition(waypoint));
    }

    for (Vector2 waypoint : chargingPath) {
        CAPTURE(waypoint.x, waypoint.y);
        CHECK(logisticsMap.isRoadPosition(waypoint));
    }

    for (Vector2 waypoint : dropoffToChargingPath) {
        CAPTURE(waypoint.x, waypoint.y);
        CHECK(logisticsMap.isRoadPosition(waypoint));
    }
}
