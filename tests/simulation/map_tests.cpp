#include "support/test_helpers.h"

#include <catch2/catch_test_macros.hpp>

#include "simulation/MapData.h"
#include "simulation/map.h"

#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>

namespace {
struct TempMapDataSource {
    std::string_view value;
};

struct TempMapDataName {
    std::string_view value;
};

class TempMapDataFile {
  public:
    TempMapDataFile(TempMapDataSource source, TempMapDataName name)
        : path_(std::filesystem::temp_directory_path() / std::string(name.value)) {
        std::ofstream file(path_);
        REQUIRE(file.is_open());
        file << source.value;
    }

    ~TempMapDataFile() {
        std::error_code error;
        std::filesystem::remove(path_, error);
    }

    [[nodiscard]] std::string pathString() const {
        return path_.string();
    }

  private:
    std::filesystem::path path_;
};
} // namespace

TEST_CASE("Map road queries", "[Navigation]") {
    LogisticsMap logisticsMap;
    logisticsMap.init();

    const Vec2 start = logisticsMap.getRobotStartPosition();
    const Vec2 offRoad = {20.0f, 20.0f};
    const Vec2 clamped = logisticsMap.clampPositionToRoad(offRoad);

    CHECK(logisticsMap.isRoadPosition(start));
    CHECK(logisticsMap.isRoadPosition(logisticsMap.getChargingStationDockPosition()));
    CHECK(!logisticsMap.isRoadPosition(offRoad));
    CHECK(logisticsMap.isRoadPosition(clamped));
    CHECK(!logisticsMap.getLagerDockPosition(LagerId::Count).has_value());
}

TEST_CASE("Map data parser accepts JSON exponent notation", "[MapData]") {
    const TempMapDataFile mapDataFile(TempMapDataSource{R"({
          "screen": { "width": 1.28e3, "height": 7.2E2 },
          "gridSize": 8e1,
          "pickupLager": 0,
          "deliveryLager": 1,
          "robotStart": { "x": 1e5, "y": 1E5 },
          "chargingStation": {
            "body": { "x": 1e-5, "y": 1E+5, "width": 2.5e-3, "height": 4.0 },
            "dockPoint": { "x": 0, "y": 0 }
          },
          "roads": [
            { "x": 0, "y": 0, "width": 1, "height": 1 }
          ],
          "warehouses": [
            { "x": 0, "y": 0, "width": 1, "height": 1 },
            { "x": 0, "y": 0, "width": 1, "height": 1 }
          ],
          "dockPoints": [
            { "x": 0, "y": 0 },
            { "x": 1, "y": 1 }
          ],
          "roadLines": [],
          "navigationNodes": [
            { "x": 0, "y": 0 },
            { "x": 1, "y": 1 }
          ],
          "navigationEdges": [
            [0, 1]
          ],
          "blockingRobotPaths": []
        })"},
                                      TempMapDataName{"fts_mapdata_exponents.json"});

    const MapData mapData = LoadMapData(mapDataFile.pathString());

    CHECK(mapData.screenWidth == 1280);
    CHECK(mapData.screenHeight == 720);
    CHECK(mapData.gridSize == 80);
    test::CheckVectorNear(mapData.robotStart, {100000.0f, 100000.0f});
    CHECK(mapData.chargingStation.body.x == Catch::Approx(0.00001f).margin(test::epsilon));
    CHECK(mapData.chargingStation.body.y == Catch::Approx(100000.0f).margin(test::epsilon));
    CHECK(mapData.chargingStation.body.width == Catch::Approx(0.0025f).margin(test::epsilon));
}

TEST_CASE("Map data parser rejects incomplete JSON exponents", "[MapData]") {
    SECTION("missing exponent digits") {
        const TempMapDataFile mapDataFile(
            TempMapDataSource{R"({"screen": { "width": 1e, "height": 720 }})"},
            TempMapDataName{"fts_mapdata_bad_exponent_missing_digits.json"});

        CHECK_THROWS_AS(LoadMapData(mapDataFile.pathString()), std::runtime_error);
    }

    SECTION("missing exponent digits after plus sign") {
        const TempMapDataFile mapDataFile(
            TempMapDataSource{R"({"screen": { "width": 1e+, "height": 720 }})"},
            TempMapDataName{"fts_mapdata_bad_exponent_plus.json"});

        CHECK_THROWS_AS(LoadMapData(mapDataFile.pathString()), std::runtime_error);
    }

    SECTION("missing exponent digits after minus sign") {
        const TempMapDataFile mapDataFile(
            TempMapDataSource{R"({"screen": { "width": 1E-, "height": 720 }})"},
            TempMapDataName{"fts_mapdata_bad_exponent_minus.json"});

        CHECK_THROWS_AS(LoadMapData(mapDataFile.pathString()), std::runtime_error);
    }

    SECTION("missing fractional exponent digits") {
        const TempMapDataFile mapDataFile(
            TempMapDataSource{R"({"screen": { "width": 2.5e-, "height": 720 }})"},
            TempMapDataName{"fts_mapdata_bad_fractional_exponent.json"});

        CHECK_THROWS_AS(LoadMapData(mapDataFile.pathString()), std::runtime_error);
    }
}
