#include "support/test_helpers.h"
#include "support/test_runner.h"
#include "support/test_suites.h"

#include "simulation/map.h"
#include "simulation/navigation.h"

#include <string>
#include <vector>

namespace {
void TestNavigationFindsWarehouseRoutes(void) {
    InitMap();

    const std::vector<Vector2> pickupPath =
        FindNavigationPath(GetRobotStartPosition(), GetLagerDockPosition(GetMapPickupLagerId()));
    const std::vector<Vector2> dropoffPath = FindNavigationPath(
        GetLagerDockPosition(GetMapPickupLagerId()), GetLagerDockPosition(GetMapDeliveryLagerId()));
    const std::vector<Vector2> chargingPath =
        FindNavigationPath(GetRobotStartPosition(), GetChargingStationDockPosition());
    const std::vector<Vector2> dropoffToChargingPath = FindNavigationPath(
        GetLagerDockPosition(GetMapDeliveryLagerId()), GetChargingStationDockPosition());
    const std::vector<Vector2> l6Path =
        FindNavigationPath(GetRobotStartPosition(), GetLagerDockPosition(LAGER_6));
    const Vector2 l6EntryWaypoint = {545.0f, 450.0f};

    test::Expect(!pickupPath.empty(), "navigation should find a path to pickup lager");
    test::Expect(!dropoffPath.empty(),
                 "navigation should find a path from pickup to dropoff lager");
    test::Expect(!chargingPath.empty(), "navigation should find a path to charging station");
    test::Expect(!dropoffToChargingPath.empty(),
                 "navigation should find a path from dropoff to charging station");
    test::Expect(!l6Path.empty(), "navigation should find a path to L6");
    test::ExpectVectorNear(pickupPath.back(), GetLagerDockPosition(GetMapPickupLagerId()),
                           "pickup path should end at pickup dock");
    test::ExpectVectorNear(dropoffPath.back(), GetLagerDockPosition(GetMapDeliveryLagerId()),
                           "dropoff path should end at dropoff dock");
    test::ExpectVectorNear(chargingPath.back(), GetChargingStationDockPosition(),
                           "charging path should end at charging station dock");
    test::ExpectVectorNear(dropoffToChargingPath.back(), GetChargingStationDockPosition(),
                           "dropoff to charging path should end at charging station dock");
    test::Expect(dropoffToChargingPath.size() <= 3,
                 "dropoff to charging path should use the direct right-side road");
    test::Expect(!test::PathContainsPoint(pickupPath, l6EntryWaypoint),
                 "pickup path should not include the L6 entry waypoint when passing by");
    test::Expect(test::PathContainsPoint(l6Path, l6EntryWaypoint),
                 "L6 path should keep the entry waypoint before turning down to the dock");

    for (Vector2 waypoint : pickupPath) {
        test::Expect(IsMapRoadPosition(waypoint), "pickup path waypoint should be on a road");
    }

    for (Vector2 waypoint : dropoffPath) {
        test::Expect(IsMapRoadPosition(waypoint), "dropoff path waypoint should be on a road");
    }

    for (Vector2 waypoint : chargingPath) {
        test::Expect(IsMapRoadPosition(waypoint), "charging path waypoint should be on a road");
    }

    for (Vector2 waypoint : dropoffToChargingPath) {
        test::Expect(IsMapRoadPosition(waypoint),
                     "dropoff to charging path waypoint should be on a road");
    }
}

const test::TestCase navigationTests[] = {
    {"Navigation finds warehouse routes", TestNavigationFindsWarehouseRoutes},
};
} // namespace

void RunNavigationTests(void) {
    test::RunTestCases(navigationTests);
}

bool RunNavigationTestByName(const std::string& name) {
    return test::RunTestCaseByName(navigationTests, name);
}
