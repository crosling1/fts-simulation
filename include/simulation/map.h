#pragma once

#include "simulation/ILogisticsMap.h"
#include "simulation/MapData.h"
#include "raylib.h"

#include <cstddef>
#include <vector>

class LogisticsMap : public ILogisticsMap {
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
    [[nodiscard]] Vector2 getChargingStationDockPosition() const override;
    [[nodiscard]] std::optional<Vector2> getLagerDockPosition(LagerId lagerId) const override;
    [[nodiscard]] std::optional<Vector2> getPickupDockPosition() const override;
    [[nodiscard]] std::optional<Vector2> getDeliveryDockPosition() const override;
    [[nodiscard]] bool isRoadPosition(Vector2 position) const override;
    [[nodiscard]] Vector2 clampPositionToRoad(Vector2 position) const override;
    [[nodiscard]] const std::vector<Vector2>& getNavigationNodes() const override;
    [[nodiscard]] const std::vector<NavigationEdge>& getNavigationEdges() const override;
    [[nodiscard]] const std::vector<BlockingRobotPath>& getBlockingRobotPaths() const;

  private:
    MapData data_;

    [[nodiscard]] Vector2 getWarehouseCenter(std::size_t index) const;
    void drawGrid() const;
    void drawRoads() const;
    void drawWarehouse(Rectangle body, int index) const;
    void drawChargingStation() const;
};
