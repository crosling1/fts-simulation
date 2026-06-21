#ifndef MAP_H
#define MAP_H

#include "simulation/MapData.h"
#include "raylib.h"

#include <cstdint>

void InitMap(void);
void DrawMap(void);
void UnloadMap(void);

typedef enum LagerId : std::uint8_t {
    LAGER_1 = 0,
    LAGER_2,
    LAGER_3,
    LAGER_4,
    LAGER_5,
    LAGER_6,
    LAGER_COUNT
} LagerId;

Vector2 GetMapPointA(void);
Vector2 GetMapPointB(void);
LagerId GetMapPickupLagerId(void);
LagerId GetMapDeliveryLagerId(void);
Vector2 GetRobotStartPosition(void);
Vector2 GetChargingStationPosition(void);
Vector2 GetChargingStationDockPosition(void);
Vector2 GetLagerPosition(LagerId lagerId);
Vector2 GetLagerDockPosition(LagerId lagerId);
bool IsMapRoadPosition(Vector2 position);
Vector2 ClampPositionToMapRoad(Vector2 position);
const std::vector<Vector2>& GetMapNavigationNodes(void);
const std::vector<NavigationEdge>& GetMapNavigationEdges(void);
const std::vector<BlockingRobotPath>& GetMapBlockingRobotPaths(void);

#endif // MAP_H
