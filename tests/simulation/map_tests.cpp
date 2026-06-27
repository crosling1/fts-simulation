#include "support/test_helpers.h"

#include <catch2/catch_test_macros.hpp>

#include "simulation/map.h"

TEST_CASE("Map road queries", "[Navigation]") {
    LogisticsMap logisticsMap;
    logisticsMap.init();

    const Vector2 start = logisticsMap.getRobotStartPosition();
    const Vector2 offRoad = {20.0f, 20.0f};
    const Vector2 clamped = logisticsMap.clampPositionToRoad(offRoad);

    CHECK(logisticsMap.isRoadPosition(start));
    CHECK(logisticsMap.isRoadPosition(logisticsMap.getChargingStationDockPosition()));
    CHECK(!logisticsMap.isRoadPosition(offRoad));
    CHECK(logisticsMap.isRoadPosition(clamped));
    CHECK(!logisticsMap.getLagerDockPosition(LagerId::Count).has_value());
}
