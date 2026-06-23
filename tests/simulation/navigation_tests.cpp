#include "support/test_helpers.h"
#include "support/test_runner.h"
#include "support/test_suites.h"

#include "simulation/map.h"
#include "simulation/navigation.h"

#include <string>
#include <vector>

namespace {
void TestNavigationFindsWarehouseRoutes(void) {
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

    test::Expect(!pickupPath.empty(), "navigation should find a path to pickup lager");
    test::Expect(!dropoffPath.empty(),
                 "navigation should find a path from pickup to dropoff lager");
    test::Expect(!chargingPath.empty(), "navigation should find a path to charging station");
    test::Expect(!dropoffToChargingPath.empty(),
                 "navigation should find a path from dropoff to charging station");
    test::Expect(!l6Path.empty(), "navigation should find a path to L6");
    test::ExpectVectorNear(pickupPath.back(),
                           logisticsMap.getLagerDockPosition(logisticsMap.getPickupLagerId()),
                           "pickup path should end at pickup dock");
    test::ExpectVectorNear(dropoffPath.back(),
                           logisticsMap.getLagerDockPosition(logisticsMap.getDeliveryLagerId()),
                           "dropoff path should end at dropoff dock");
    test::ExpectVectorNear(chargingPath.back(), logisticsMap.getChargingStationDockPosition(),
                           "charging path should end at charging station dock");
    test::ExpectVectorNear(dropoffToChargingPath.back(),
                           logisticsMap.getChargingStationDockPosition(),
                           "dropoff to charging path should end at charging station dock");
    test::Expect(dropoffToChargingPath.size() <= 3,
                 "dropoff to charging path should use the direct right-side road");
    test::Expect(!test::PathContainsPoint(pickupPath, l6EntryWaypoint),
                 "pickup path should not include the L6 entry waypoint when passing by");
    test::Expect(test::PathContainsPoint(l6Path, l6EntryWaypoint),
                 "L6 path should keep the entry waypoint before turning down to the dock");

    for (Vector2 waypoint : pickupPath) {
        test::Expect(logisticsMap.isRoadPosition(waypoint),
                     "pickup path waypoint should be on a road");
    }

    for (Vector2 waypoint : dropoffPath) {
        test::Expect(logisticsMap.isRoadPosition(waypoint),
                     "dropoff path waypoint should be on a road");
    }

    for (Vector2 waypoint : chargingPath) {
        test::Expect(logisticsMap.isRoadPosition(waypoint),
                     "charging path waypoint should be on a road");
    }

    for (Vector2 waypoint : dropoffToChargingPath) {
        test::Expect(logisticsMap.isRoadPosition(waypoint),
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
