#include "map.h"

#include <algorithm>

class LogisticsMap {
  public:
    void Init(void) {
        // If map initialization needs resources in future, allocate them here.
    }

    void Draw(void) const {
        ClearBackground(RAYWHITE);

        DrawGrid();
        DrawRoads();

        for (int i = 0; i < warehouseCount; i++) {
            DrawCircleV(dockPoints[i], 5.0f, DARKGRAY);
            DrawWarehouse(warehouses[i], i);
        }

        DrawCircleV(robotStart, 20, ORANGE);
        DrawCircleLines((int)robotStart.x, (int)robotStart.y, 24.0f, BROWN);
        DrawText("Robot Start", (int)robotStart.x - 47, (int)robotStart.y + 34, 18, DARKGRAY);

        DrawText("Robot Logistics Map", 20, 20, 28, DARKGRAY);
        DrawText("A: Pickup lager", 20, 60, 18, DARKGREEN);
        DrawText("B: Delivery lager", 20, 84, 18, MAROON);
        DrawText("L1~L6: Lager / warehouses", 20, 108, 18, BLUE);
        DrawText("Orange circle: Robot start", 20, 132, 18, ORANGE);
        DrawText("Gray rectangles: road network to each lager", 20, 156, 18, DARKGRAY);
    }

    void Unload(void) {
        // No resources allocated yet.
    }

    Vector2 GetPointA(void) const {
        return GetWarehouseCenter(pointAIndex);
    }

    Vector2 GetPointB(void) const {
        return GetWarehouseCenter(pointBIndex);
    }

    Vector2 GetRobotStart(void) const {
        return robotStart;
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

        return dockPoints[lagerId];
    }

    bool IsRoadPosition(Vector2 position) const {
        for (Rectangle road : roads) {
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

        for (Rectangle road : roads) {
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

  private:
    static constexpr int screenWidth = 1280;
    static constexpr int screenHeight = 720;
    static constexpr int gridSize = 80;
    static constexpr int pointAIndex = 1;
    static constexpr int pointBIndex = 4;

    static constexpr int roadCount = 14;
    static constexpr int warehouseCount = LAGER_COUNT;

    Vector2 robotStart = {140.0f, 620.0f};

    Rectangle roads[roadCount] = {
        {180.0f, 130.0f, 40.0f, 510.0f}, {180.0f, 430.0f, 860.0f, 40.0f},
        {780.0f, 130.0f, 40.0f, 320.0f}, {520.0f, 130.0f, 40.0f, 220.0f},
        {520.0f, 330.0f, 420.0f, 40.0f}, {320.0f, 250.0f, 500.0f, 40.0f},
        {940.0f, 330.0f, 40.0f, 246.0f}, {320.0f, 270.0f, 40.0f, 55.0f},
        {430.0f, 224.0f, 40.0f, 46.0f},  {655.0f, 224.0f, 40.0f, 46.0f},
        {800.0f, 237.0f, 50.0f, 40.0f},  {920.0f, 536.0f, 40.0f, 40.0f},
        {525.0f, 450.0f, 40.0f, 55.0f},  {120.0f, 600.0f, 80.0f, 40.0f},
    };

    Rectangle warehouses[warehouseCount] = {
        {300.0f, 325.0f, 90.0f, 64.0f}, {405.0f, 160.0f, 90.0f, 64.0f},
        {630.0f, 160.0f, 90.0f, 64.0f}, {850.0f, 215.0f, 90.0f, 64.0f},
        {840.0f, 520.0f, 90.0f, 64.0f}, {500.0f, 505.0f, 90.0f, 64.0f},
    };

    Vector2 dockPoints[warehouseCount] = {
        {340.0f, 325.0f}, {450.0f, 224.0f}, {675.0f, 224.0f},
        {850.0f, 257.0f}, {930.0f, 556.0f}, {545.0f, 505.0f},
    };

    Vector2 GetWarehouseCenter(int index) const {
        Rectangle warehouse = warehouses[index];

        return (Vector2){
            warehouse.x + warehouse.width * 0.5f,
            warehouse.y + warehouse.height * 0.5f,
        };
    }

    void DrawGrid(void) const {
        for (int x = 0; x <= screenWidth; x += gridSize) {
            DrawLine(x, 0, x, screenHeight, Fade(LIGHTGRAY, 0.55f));
        }

        for (int y = 0; y <= screenHeight; y += gridSize) {
            DrawLine(0, y, screenWidth, y, Fade(LIGHTGRAY, 0.55f));
        }
    }

    void DrawRoads(void) const {
        for (int i = 0; i < roadCount; i++) {
            DrawRectangleRec(roads[i], GRAY);
        }

        DrawLineEx((Vector2){200.0f, 150.0f}, (Vector2){200.0f, 620.0f}, 4.0f, DARKGRAY);
        DrawLineEx((Vector2){200.0f, 450.0f}, (Vector2){1040.0f, 450.0f}, 4.0f, DARKGRAY);
        DrawLineEx((Vector2){800.0f, 150.0f}, (Vector2){800.0f, 470.0f}, 4.0f, DARKGRAY);
        DrawLineEx((Vector2){540.0f, 150.0f}, (Vector2){540.0f, 350.0f}, 4.0f, DARKGRAY);
        DrawLineEx((Vector2){340.0f, 270.0f}, (Vector2){800.0f, 270.0f}, 4.0f, DARKGRAY);
        DrawLineEx((Vector2){540.0f, 350.0f}, (Vector2){960.0f, 350.0f}, 4.0f, DARKGRAY);
        DrawLineEx((Vector2){960.0f, 350.0f}, (Vector2){960.0f, 556.0f}, 4.0f, DARKGRAY);
        DrawLineEx((Vector2){340.0f, 270.0f}, (Vector2){340.0f, 325.0f}, 4.0f, DARKGRAY);
        DrawLineEx((Vector2){450.0f, 224.0f}, (Vector2){450.0f, 270.0f}, 4.0f, DARKGRAY);
        DrawLineEx((Vector2){675.0f, 224.0f}, (Vector2){675.0f, 270.0f}, 4.0f, DARKGRAY);
        DrawLineEx((Vector2){800.0f, 257.0f}, (Vector2){850.0f, 257.0f}, 4.0f, DARKGRAY);
        DrawLineEx((Vector2){930.0f, 556.0f}, (Vector2){960.0f, 556.0f}, 4.0f, DARKGRAY);
        DrawLineEx((Vector2){545.0f, 450.0f}, (Vector2){545.0f, 505.0f}, 4.0f, DARKGRAY);
        DrawLineEx((Vector2){140.0f, 620.0f}, (Vector2){200.0f, 620.0f}, 4.0f, DARKGRAY);
    }

    void DrawWarehouse(Rectangle body, int index) const {
        bool isPointA = index == pointAIndex;
        bool isPointB = index == pointBIndex;
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

    bool IsValidLagerId(LagerId lagerId) const {
        return lagerId >= LAGER_1 && lagerId < LAGER_COUNT;
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

Vector2 GetRobotStartPosition(void) {
    return map.GetRobotStart();
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
