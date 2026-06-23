#pragma once

#include "raylib.h"

#include <cstddef>
#include <string>
#include <vector>

struct RoadLine {
    Vector2 start;
    Vector2 end;
    float width;
};

struct NavigationEdge {
    std::size_t from;
    std::size_t to;
};

struct BlockingRobotPath {
    float speedMultiplier;
    std::vector<Vector2> points;
};

struct ChargingStationData {
    Rectangle body;
    Vector2 dockPoint;
};

struct MapData {
    int screenWidth;
    int screenHeight;
    int gridSize;
    int pickupLagerIndex;
    int deliveryLagerIndex;
    Vector2 robotStart;
    ChargingStationData chargingStation;
    std::vector<Rectangle> roads;
    std::vector<Rectangle> warehouses;
    std::vector<Vector2> dockPoints;
    std::vector<RoadLine> roadLines;
    std::vector<Vector2> navigationNodes;
    std::vector<NavigationEdge> navigationEdges;
    std::vector<BlockingRobotPath> blockingRobotPaths;
};

MapData LoadMapData(const std::string& path);
