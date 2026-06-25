#pragma once

#include "simulation/MapData.h"
#include "raylib.h"

#include <cstdint>
#include <vector>

typedef enum LagerId : std::uint8_t {
    LAGER_1 = 0,
    LAGER_2,
    LAGER_3,
    LAGER_4,
    LAGER_5,
    LAGER_6,
    LAGER_COUNT
} LagerId;

class LogisticsMap {
  public:
    void init();
    void draw() const;
    void unload();

    [[nodiscard]] Vector2 getPointA() const;
    [[nodiscard]] Vector2 getPointB() const;
    [[nodiscard]] LagerId getPickupLagerId() const;
    [[nodiscard]] LagerId getDeliveryLagerId() const;
    [[nodiscard]] Vector2 getRobotStartPosition() const;
    [[nodiscard]] Vector2 getChargingStationPosition() const;
    [[nodiscard]] Vector2 getChargingStationDockPosition() const;
    [[nodiscard]] Vector2 getLagerPosition(LagerId lagerId) const;
    [[nodiscard]] Vector2 getLagerDockPosition(LagerId lagerId) const;
    [[nodiscard]] bool isRoadPosition(Vector2 position) const;
    [[nodiscard]] Vector2 clampPositionToRoad(Vector2 position) const;
    [[nodiscard]] const std::vector<Vector2>& getNavigationNodes() const;
    [[nodiscard]] const std::vector<NavigationEdge>& getNavigationEdges() const;
    [[nodiscard]] const std::vector<BlockingRobotPath>& getBlockingRobotPaths() const;

  private:
    MapData data_;

    [[nodiscard]] Vector2 getWarehouseCenter(int index) const;
    void drawGrid() const;
    void drawRoads() const;
    void drawWarehouse(Rectangle body, int index) const;
    void drawChargingStation() const;
    [[nodiscard]] bool isValidLagerId(LagerId lagerId) const;
};
