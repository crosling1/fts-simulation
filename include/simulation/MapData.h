#pragma once

#include "simulation/Geometry.h"

#include <cstddef>
#include <string>
#include <vector>

struct RoadLine {
    Vec2 start;
    Vec2 end;
    float width;
};

struct NavigationEdge {
    std::size_t from;
    std::size_t to;
};

struct BlockingRobotPath {
    float speedMultiplier;
    std::vector<Vec2> points;
};

struct ChargingStationData {
    Rect body;
    Vec2 dockPoint;
};

struct MapData {
    int screenWidth;
    int screenHeight;
    int gridSize;
    int pickupLagerIndex;
    int deliveryLagerIndex;
    Vec2 robotStart;
    ChargingStationData chargingStation;
    std::vector<Rect> roads;
    std::vector<Rect> warehouses;
    std::vector<Vec2> dockPoints;
    std::vector<RoadLine> roadLines;
    std::vector<Vec2> navigationNodes;
    std::vector<NavigationEdge> navigationEdges;
    std::vector<BlockingRobotPath> blockingRobotPaths;
};

MapData LoadMapData(const std::string& path);
