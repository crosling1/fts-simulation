#include "simulation/map.h"

#include <algorithm>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
MapData LoadDefaultMapData(void) {
    try {
        return LoadMapData("assets/maps/warehouse_map.json");
    } catch (const std::exception&) {
        return LoadMapData("../assets/maps/warehouse_map.json");
    }
}
} // namespace

class LogisticsMap {
  public:
    void Init(void) {
        data = LoadDefaultMapData();
    }

    void Draw(void) const {
        ClearBackground(RAYWHITE);

        DrawGrid();
        DrawRoads();

        for (std::size_t i = 0; i < data.warehouses.size(); i++) {
            DrawCircleV(data.dockPoints[i], 5.0f, DARKGRAY);
            DrawWarehouse(data.warehouses[i], static_cast<int>(i));
        }

        DrawChargingStation();

        DrawCircleV(data.robotStart, 20, ORANGE);
        DrawCircleLines((int)data.robotStart.x, (int)data.robotStart.y, 24.0f, BROWN);
        DrawText("Robot Start", (int)data.robotStart.x - 47, (int)data.robotStart.y + 34, 18,
                 DARKGRAY);

        DrawText("Robot Logistics Map", 20, 20, 28, DARKGRAY);
        DrawText("A: Pickup lager", 20, 60, 18, DARKGREEN);
        DrawText("B: Delivery lager", 20, 84, 18, MAROON);
        DrawText("L1~L6: Lager / warehouses", 20, 108, 18, BLUE);
        DrawText("Orange circle: Robot start", 20, 132, 18, ORANGE);
        DrawText("Gray rectangles: road network to each lager", 20, 156, 18, DARKGRAY);
        DrawText("C: Charging station", 20, 180, 18, DARKPURPLE);
    }

    void Unload(void) {
        // No resources allocated yet.
    }

    Vector2 GetPointA(void) const {
        return GetWarehouseCenter(data.pickupLagerIndex);
    }

    Vector2 GetPointB(void) const {
        return GetWarehouseCenter(data.deliveryLagerIndex);
    }

    LagerId GetPickupLagerId(void) const {
        return static_cast<LagerId>(data.pickupLagerIndex);
    }

    LagerId GetDeliveryLagerId(void) const {
        return static_cast<LagerId>(data.deliveryLagerIndex);
    }

    Vector2 GetRobotStart(void) const {
        return data.robotStart;
    }

    Vector2 GetChargingStationPosition(void) const {
        return {
            data.chargingStation.body.x + data.chargingStation.body.width * 0.5f,
            data.chargingStation.body.y + data.chargingStation.body.height * 0.5f,
        };
    }

    Vector2 GetChargingStationDockPosition(void) const {
        return data.chargingStation.dockPoint;
    }

    Vector2 GetLagerPosition(LagerId lagerId) const {
        if (!IsValidLagerId(lagerId)) {
            return (Vector2){0.0f, 0.0f};
        }

        return GetWarehouseCenter(lagerId);
    }

    Vector2 GetLagerDockPosition(LagerId lagerId) const {
        if (!IsValidLagerId(lagerId)) {
            return (Vector2){0.0f, 0.0f};
        }

        return data.dockPoints[lagerId];
    }

    bool IsRoadPosition(Vector2 position) const {
        for (Rectangle road : data.roads) {
            if (CheckCollisionPointRec(position, road)) {
                return true;
            }
        }

        return false;
    }

    Vector2 ClampPositionToRoad(Vector2 position) const {
        Vector2 closestPosition = position;
        float closestDistanceSquared = 0.0f;
        bool hasClosestPosition = false;

        for (Rectangle road : data.roads) {
            const Vector2 clampedPosition = {
                std::clamp(position.x, road.x, road.x + road.width),
                std::clamp(position.y, road.y, road.y + road.height),
            };
            const float deltaX = position.x - clampedPosition.x;
            const float deltaY = position.y - clampedPosition.y;
            const float distanceSquared = (deltaX * deltaX) + (deltaY * deltaY);

            if (!hasClosestPosition || distanceSquared < closestDistanceSquared) {
                closestPosition = clampedPosition;
                closestDistanceSquared = distanceSquared;
                hasClosestPosition = true;
            }
        }

        return closestPosition;
    }

    const std::vector<Vector2>& GetNavigationNodes(void) const {
        return data.navigationNodes;
    }

    const std::vector<NavigationEdge>& GetNavigationEdges(void) const {
        return data.navigationEdges;
    }

    const std::vector<BlockingRobotPath>& GetBlockingRobotPaths(void) const {
        return data.blockingRobotPaths;
    }

  private:
    MapData data;

    Vector2 GetWarehouseCenter(int index) const {
        Rectangle warehouse = data.warehouses[index];

        return (Vector2){
            warehouse.x + warehouse.width * 0.5f,
            warehouse.y + warehouse.height * 0.5f,
        };
    }

    void DrawGrid(void) const {
        for (int x = 0; x <= data.screenWidth; x += data.gridSize) {
            DrawLine(x, 0, x, data.screenHeight, Fade(LIGHTGRAY, 0.55f));
        }

        for (int y = 0; y <= data.screenHeight; y += data.gridSize) {
            DrawLine(0, y, data.screenWidth, y, Fade(LIGHTGRAY, 0.55f));
        }
    }

    void DrawRoads(void) const {
        for (Rectangle road : data.roads) {
            DrawRectangleRec(road, GRAY);
        }

        for (RoadLine roadLine : data.roadLines) {
            DrawLineEx(roadLine.start, roadLine.end, roadLine.width, DARKGRAY);
        }
    }

    void DrawWarehouse(Rectangle body, int index) const {
        const bool isPointA = index == data.pickupLagerIndex;
        const bool isPointB = index == data.deliveryLagerIndex;
        Color fillColor = isPointA ? Fade(GREEN, 0.55f) : isPointB ? Fade(RED, 0.45f) : SKYBLUE;
        Color lineColor = isPointA ? DARKGREEN : isPointB ? MAROON : BLUE;

        DrawRectangleRec(body, fillColor);
        DrawRectangleLinesEx(body, 3.0f, lineColor);
        DrawRectangle((int)body.x + 10, (int)body.y + 43, 70, 12, Fade(lineColor, 0.35f));
        DrawText(TextFormat("L%d", index + 1), (int)body.x + 28, (int)body.y + 16, 24, DARKBLUE);

        if (isPointA || isPointB) {
            const char* label = isPointA ? "A" : "B";
            Color badgeColor = isPointA ? DARKGREEN : MAROON;

            DrawCircle((int)body.x + 78, (int)body.y + 14, 15.0f, badgeColor);
            DrawText(label, (int)body.x + 70, (int)body.y + 3, 22, WHITE);
        }
    }

    void DrawChargingStation(void) const {
        const Rectangle body = data.chargingStation.body;
        const Vector2 dock = data.chargingStation.dockPoint;
        const Vector2 connectorEnd = {body.x, body.y + body.height * 0.5f};

        DrawLineEx(dock, connectorEnd, 5.0f, DARKPURPLE);
        DrawCircleV(dock, 6.0f, DARKPURPLE);

        DrawRectangleRec(body, Fade(PURPLE, 0.25f));
        DrawRectangleLinesEx(body, 3.0f, DARKPURPLE);
        DrawText("C", (int)body.x + 12, (int)body.y + 10, 28, DARKPURPLE);
        DrawText("CHARGE", (int)body.x + 14, (int)body.y + 52, 14, DARKPURPLE);

        const Rectangle batteryBody = {body.x + 38.0f, body.y + 18.0f, 30.0f, 18.0f};
        const Rectangle batteryTip = {body.x + 68.0f, body.y + 23.0f, 5.0f, 8.0f};
        DrawRectangleRec(batteryBody, RAYWHITE);
        DrawRectangleLinesEx(batteryBody, 2.0f, DARKPURPLE);
        DrawRectangleRec(batteryTip, DARKPURPLE);
        DrawRectangle((int)batteryBody.x + 4, (int)batteryBody.y + 4, 18, 10, GREEN);
    }

    bool IsValidLagerId(LagerId lagerId) const {
        return lagerId >= LAGER_1 && static_cast<std::size_t>(lagerId) < data.warehouses.size();
    }
};

static LogisticsMap map;

void InitMap(void) {
    map.Init();
}

void DrawMap(void) {
    map.Draw();
}

void UnloadMap(void) {
    map.Unload();
}

Vector2 GetMapPointA(void) {
    return map.GetPointA();
}

Vector2 GetMapPointB(void) {
    return map.GetPointB();
}

LagerId GetMapPickupLagerId(void) {
    return map.GetPickupLagerId();
}

LagerId GetMapDeliveryLagerId(void) {
    return map.GetDeliveryLagerId();
}

Vector2 GetRobotStartPosition(void) {
    return map.GetRobotStart();
}

Vector2 GetChargingStationPosition(void) {
    return map.GetChargingStationPosition();
}

Vector2 GetChargingStationDockPosition(void) {
    return map.GetChargingStationDockPosition();
}

Vector2 GetLagerPosition(LagerId lagerId) {
    return map.GetLagerPosition(lagerId);
}

Vector2 GetLagerDockPosition(LagerId lagerId) {
    return map.GetLagerDockPosition(lagerId);
}

bool IsMapRoadPosition(Vector2 position) {
    return map.IsRoadPosition(position);
}

Vector2 ClampPositionToMapRoad(Vector2 position) {
    return map.ClampPositionToRoad(position);
}

const std::vector<Vector2>& GetMapNavigationNodes(void) {
    return map.GetNavigationNodes();
}

const std::vector<NavigationEdge>& GetMapNavigationEdges(void) {
    return map.GetNavigationEdges();
}

const std::vector<BlockingRobotPath>& GetMapBlockingRobotPaths(void) {
    return map.GetBlockingRobotPaths();
}
