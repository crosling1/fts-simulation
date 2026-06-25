#include "support/test_helpers.h"

#include <catch2/catch_test_macros.hpp>

#include "simulation/map.h"
#include "simulation/navigation.h"

#include <vector>

TEST_CASE("Navigation finds warehouse routes", "[Navigation]") {
    LogisticsMap logisticsMap;
    logisticsMap.init();

    const std::vector<Vector2> pickupPath =
        FindNavigationPath(logisticsMap, logisticsMap.getRobotStartPosition(),
                           logisticsMap.getLagerDockPosition(logisticsMap.getPickupLagerId()));
    const std::vector<Vector2> dropoffPath = FindNavigationPath(
        logisticsMap, logisticsMap.getLagerDockPosition(logisticsMap.getPickupLagerId()),
        logisticsMap.getLagerDockPosition(logisticsMap.getDeliveryLagerId()));
    const std::vector<Vector2> chargingPath =
        FindNavigationPath(logisticsMap, logisticsMap.getRobotStartPosition(),
                           logisticsMap.getChargingStationDockPosition());
    const std::vector<Vector2> dropoffToChargingPath = FindNavigationPath(
        logisticsMap, logisticsMap.getLagerDockPosition(logisticsMap.getDeliveryLagerId()),
        logisticsMap.getChargingStationDockPosition());
    const std::vector<Vector2> l6Path =
        FindNavigationPath(logisticsMap, logisticsMap.getRobotStartPosition(),
                           logisticsMap.getLagerDockPosition(LAGER_6));
    const Vector2 l6EntryWaypoint = {545.0f, 450.0f};

    REQUIRE_FALSE(pickupPath.empty());
    REQUIRE_FALSE(dropoffPath.empty());
    REQUIRE_FALSE(chargingPath.empty());
    REQUIRE_FALSE(dropoffToChargingPath.empty());
    REQUIRE_FALSE(l6Path.empty());

    test::CheckVectorNear(pickupPath.back(),
                          logisticsMap.getLagerDockPosition(logisticsMap.getPickupLagerId()));
    test::CheckVectorNear(dropoffPath.back(),
                          logisticsMap.getLagerDockPosition(logisticsMap.getDeliveryLagerId()));
    test::CheckVectorNear(chargingPath.back(), logisticsMap.getChargingStationDockPosition());
    test::CheckVectorNear(dropoffToChargingPath.back(),
                          logisticsMap.getChargingStationDockPosition());
    CHECK(dropoffToChargingPath.size() <= 3);
    CHECK_FALSE(test::PathContainsPoint(pickupPath, l6EntryWaypoint));
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
