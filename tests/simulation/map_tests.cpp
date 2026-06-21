#include "support/test_helpers.h"
#include "support/test_runner.h"
#include "support/test_suites.h"

#include "simulation/map.h"

#include <string>

namespace {
void TestMapRoadQueries(void) {
    InitMap();

    const Vector2 start = GetRobotStartPosition();
    const Vector2 offRoad = {20.0f, 20.0f};
    const Vector2 clamped = ClampPositionToMapRoad(offRoad);

    test::Expect(IsMapRoadPosition(start), "robot start should be on a road");
    test::Expect(IsMapRoadPosition(GetChargingStationDockPosition()),
                 "charging station dock should be on a road");
    test::Expect(!IsMapRoadPosition(offRoad), "off-road point should not be on a road");
    test::Expect(IsMapRoadPosition(clamped), "clamped off-road point should land on a road");
    test::ExpectVectorNear(GetLagerDockPosition(static_cast<LagerId>(LAGER_COUNT)), {0.0f, 0.0f},
                           "invalid lager id should return origin");
}

const test::TestCase mapTests[] = {
    {"Map road queries", TestMapRoadQueries},
};
} // namespace

void RunMapTests(void) {
    test::RunTestCases(mapTests);
}

bool RunMapTestByName(const std::string& name) {
    return test::RunTestCaseByName(mapTests, name);
}
