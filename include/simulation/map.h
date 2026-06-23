#ifndef MAP_H
#define MAP_H

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
    void init(void);
    void draw(void) const;
    void unload(void);

    Vector2 getPointA(void) const;
    Vector2 getPointB(void) const;
    LagerId getPickupLagerId(void) const;
    LagerId getDeliveryLagerId(void) const;
    Vector2 getRobotStartPosition(void) const;
    Vector2 getChargingStationPosition(void) const;
    Vector2 getChargingStationDockPosition(void) const;
    Vector2 getLagerPosition(LagerId lagerId) const;
    Vector2 getLagerDockPosition(LagerId lagerId) const;
    bool isRoadPosition(Vector2 position) const;
    Vector2 clampPositionToRoad(Vector2 position) const;
    const std::vector<Vector2>& getNavigationNodes(void) const;
    const std::vector<NavigationEdge>& getNavigationEdges(void) const;
    const std::vector<BlockingRobotPath>& getBlockingRobotPaths(void) const;

  private:
    MapData data_;

    Vector2 getWarehouseCenter(int index) const;
    void drawGrid(void) const;
    void drawRoads(void) const;
    void drawWarehouse(Rectangle body, int index) const;
    void drawChargingStation(void) const;
    bool isValidLagerId(LagerId lagerId) const;
};

#endif // MAP_H
