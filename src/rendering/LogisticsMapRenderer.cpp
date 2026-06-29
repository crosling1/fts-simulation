#include "rendering/SimulationRenderer.h"

#include "rendering/RaylibGeometry.h"
#include "simulation/map.h"

namespace {
void DrawGrid(const MapData& data) {
    for (int x = 0; x <= data.screenWidth; x += data.gridSize) {
        DrawLine(x, 0, x, data.screenHeight, Fade(LIGHTGRAY, 0.55f));
    }

    for (int y = 0; y <= data.screenHeight; y += data.gridSize) {
        DrawLine(0, y, data.screenWidth, y, Fade(LIGHTGRAY, 0.55f));
    }
}

void DrawRoads(const MapData& data) {
    for (Rect road : data.roads) {
        DrawRectangleRec(ToRaylib(road), GRAY);
    }

    for (RoadLine roadLine : data.roadLines) {
        DrawLineEx(ToRaylib(roadLine.start), ToRaylib(roadLine.end), roadLine.width, DARKGRAY);
    }
}

void DrawWarehouse(const MapData& data, Rect body, int index) {
    const bool isPickupLager = index == data.pickupLagerIndex;
    const bool isDeliveryLager = index == data.deliveryLagerIndex;
    Color fillColor = isPickupLager     ? Fade(GREEN, 0.55f)
                      : isDeliveryLager ? Fade(RED, 0.45f)
                                        : SKYBLUE;
    Color lineColor = isPickupLager ? DARKGREEN : isDeliveryLager ? MAROON : BLUE;

    DrawRectangleRec(ToRaylib(body), fillColor);
    DrawRectangleLinesEx(ToRaylib(body), 3.0f, lineColor);
    DrawRectangle(static_cast<int>(body.x) + 10, static_cast<int>(body.y) + 43, 70, 12,
                  Fade(lineColor, 0.35f));
    DrawText(TextFormat("L%d", index + 1), static_cast<int>(body.x) + 28,
             static_cast<int>(body.y) + 16, 24, DARKBLUE);

    if (isPickupLager || isDeliveryLager) {
        const char* label = isPickupLager ? "A" : "B";
        Color badgeColor = isPickupLager ? DARKGREEN : MAROON;

        DrawCircle(static_cast<int>(body.x) + 78, static_cast<int>(body.y) + 14, 15.0f, badgeColor);
        DrawText(label, static_cast<int>(body.x) + 70, static_cast<int>(body.y) + 3, 22, WHITE);
    }
}

void DrawChargingStation(const MapData& data) {
    const Rect body = data.chargingStation.body;
    const Vec2 dock = data.chargingStation.dockPoint;
    const Vec2 connectorEnd = {body.x, body.y + body.height * 0.5f};

    DrawLineEx(ToRaylib(dock), ToRaylib(connectorEnd), 5.0f, DARKPURPLE);
    DrawCircleV(ToRaylib(dock), 6.0f, DARKPURPLE);

    DrawRectangleRec(ToRaylib(body), Fade(PURPLE, 0.25f));
    DrawRectangleLinesEx(ToRaylib(body), 3.0f, DARKPURPLE);
    DrawText("C", static_cast<int>(body.x) + 12, static_cast<int>(body.y) + 10, 28, DARKPURPLE);
    DrawText("CHARGE", static_cast<int>(body.x) + 14, static_cast<int>(body.y) + 52, 14,
             DARKPURPLE);

    const Rect batteryBody = {body.x + 38.0f, body.y + 18.0f, 30.0f, 18.0f};
    const Rect batteryTip = {body.x + 68.0f, body.y + 23.0f, 5.0f, 8.0f};
    DrawRectangleRec(ToRaylib(batteryBody), RAYWHITE);
    DrawRectangleLinesEx(ToRaylib(batteryBody), 2.0f, DARKPURPLE);
    DrawRectangleRec(ToRaylib(batteryTip), DARKPURPLE);
    DrawRectangle(static_cast<int>(batteryBody.x) + 4, static_cast<int>(batteryBody.y) + 4, 18, 10,
                  GREEN);
}
} // namespace

void DrawLogisticsMap(const LogisticsMap& logisticsMap) {
    const MapData& data = logisticsMap.getData();

    ClearBackground(RAYWHITE);

    DrawGrid(data);
    DrawRoads(data);

    for (std::size_t i = 0; i < data.warehouses.size(); i++) {
        DrawCircleV(ToRaylib(data.dockPoints[i]), 5.0f, DARKGRAY);
        DrawWarehouse(data, data.warehouses[i], static_cast<int>(i));
    }

    DrawChargingStation(data);

    DrawCircleV(ToRaylib(data.robotStart), 20, ORANGE);
    DrawCircleLines(static_cast<int>(data.robotStart.x), static_cast<int>(data.robotStart.y), 24.0f,
                    BROWN);
    DrawText("Robot Start", static_cast<int>(data.robotStart.x) - 47,
             static_cast<int>(data.robotStart.y) + 34, 18, DARKGRAY);
}
