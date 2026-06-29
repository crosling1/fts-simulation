#include "support/test_helpers.h"

#include <catch2/catch_test_macros.hpp>

#include "simulation/map.h"
#include "simulation/navigation.h"

#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace {
template <typename T>
[[nodiscard]] T RequireOptionalValue(std::optional<T> value, std::string_view message) {
    if (value.has_value()) {
        return value.value();
    }

    throw std::runtime_error(std::string(message));
}

bool SamePosition(Vec2 left, Vec2 right) {
    return test::AlmostEqual(left.x, right.x) && test::AlmostEqual(left.y, right.y);
}

std::optional<Vec2> FindConnectedNavigationNode(const LogisticsMap& logisticsMap,
                                                Vec2 navigationNode) {
    const std::vector<Vec2>& navigationNodes = logisticsMap.getNavigationNodes();

    for (NavigationEdge edge : logisticsMap.getNavigationEdges()) {
        if (SamePosition(navigationNodes[edge.from], navigationNode)) {
            return navigationNodes[edge.to];
        }

        if (SamePosition(navigationNodes[edge.to], navigationNode)) {
            return navigationNodes[edge.from];
        }
    }

    return std::nullopt;
}
} // namespace

TEST_CASE("Navigation finds warehouse routes", "[Navigation]") {
    LogisticsMap logisticsMap;
    logisticsMap.init();

    const Vec2 pickupDockPosition =
        RequireOptionalValue(logisticsMap.getLagerDockPosition(logisticsMap.getPickupLagerId()),
                             "Expected pickup dock position to exist");
    const Vec2 deliveryDockPosition =
        RequireOptionalValue(logisticsMap.getLagerDockPosition(logisticsMap.getDeliveryLagerId()),
                             "Expected delivery dock position to exist");
    const Vec2 l6DockPosition = RequireOptionalValue(logisticsMap.getLagerDockPosition(LagerId::L6),
                                                     "Expected L6 dock position to exist");
    const Vec2 l6EntryWaypoint =
        RequireOptionalValue(FindConnectedNavigationNode(logisticsMap, l6DockPosition),
                             "Expected L6 dock position to have a connected navigation node");

    const std::vector<Vec2> pickupPath =
        FindNavigationPath(logisticsMap, logisticsMap.getRobotStartPosition(), pickupDockPosition);
    const std::vector<Vec2> dropoffPath =
        FindNavigationPath(logisticsMap, pickupDockPosition, deliveryDockPosition);
    const std::vector<Vec2> chargingPath =
        FindNavigationPath(logisticsMap, logisticsMap.getRobotStartPosition(),
                           logisticsMap.getChargingStationDockPosition());
    const std::vector<Vec2> dropoffToChargingPath = FindNavigationPath(
        logisticsMap, deliveryDockPosition, logisticsMap.getChargingStationDockPosition());
    const std::vector<Vec2> l6Path =
        FindNavigationPath(logisticsMap, logisticsMap.getRobotStartPosition(), l6DockPosition);

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

    for (Vec2 waypoint : pickupPath) {
        CAPTURE(waypoint.x, waypoint.y);
        CHECK(logisticsMap.isRoadPosition(waypoint));
    }

    for (Vec2 waypoint : dropoffPath) {
        CAPTURE(waypoint.x, waypoint.y);
        CHECK(logisticsMap.isRoadPosition(waypoint));
    }

    for (Vec2 waypoint : chargingPath) {
        CAPTURE(waypoint.x, waypoint.y);
        CHECK(logisticsMap.isRoadPosition(waypoint));
    }

    for (Vec2 waypoint : dropoffToChargingPath) {
        CAPTURE(waypoint.x, waypoint.y);
        CHECK(logisticsMap.isRoadPosition(waypoint));
    }
}
