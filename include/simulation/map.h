#pragma once

#include "simulation/ILogisticsMap.h"
#include "simulation/MapData.h"
#include "simulation/Geometry.h"

#include <cstddef>
#include <vector>

class LogisticsMap : public ILogisticsMap {
  public:
    void init();
    void draw() const;
    void unload();

    [[nodiscard]] LagerId getPickupLagerId() const;
    [[nodiscard]] LagerId getDeliveryLagerId() const;
    [[nodiscard]] Vec2 getRobotStartPosition() const;
    [[nodiscard]] Vec2 getChargingStationPosition() const;
    [[nodiscard]] Vec2 getChargingStationDockPosition() const override;
    [[nodiscard]] std::optional<Vec2> getLagerDockPosition(LagerId lagerId) const override;
    [[nodiscard]] std::optional<Vec2> getPickupDockPosition() const override;
    [[nodiscard]] std::optional<Vec2> getDeliveryDockPosition() const override;
    [[nodiscard]] bool isRoadPosition(Vec2 position) const override;
    [[nodiscard]] Vec2 clampPositionToRoad(Vec2 position) const override;
    [[nodiscard]] const std::vector<Vec2>& getNavigationNodes() const override;
    [[nodiscard]] const std::vector<NavigationEdge>& getNavigationEdges() const override;
    [[nodiscard]] const std::vector<BlockingRobotPath>& getBlockingRobotPaths() const;

  private:
    MapData data_;

    [[nodiscard]] Vec2 getWarehouseCenter(std::size_t index) const;
    void drawGrid() const;
    void drawRoads() const;
    void drawWarehouse(Rect body, int index) const;
    void drawChargingStation() const;
};
