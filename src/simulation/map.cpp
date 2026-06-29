#include "simulation/map.h"

#include "rendering/RaylibGeometry.h"

#include <algorithm>
#include <array>
#include <filesystem>
#include <stdexcept>

namespace {

constexpr const char* kMapDataPath = "assets/maps/warehouse_map.json";
constexpr const char* kParentMapDataPath = "../assets/maps/warehouse_map.json";

[[nodiscard]] MapData LoadDefaultMapData() {
    const std::array<std::filesystem::path, 2> candidates = {
        std::filesystem::path{kMapDataPath},
        std::filesystem::path{kParentMapDataPath},
    };

    for (const std::filesystem::path& path : candidates) {
        if (std::filesystem::exists(path)) {
            return LoadMapData(path.string());
        }
    }

    throw std::runtime_error("Could not find warehouse map data file");
}

[[nodiscard]] bool ContainsPoint(Rect rect, Vec2 point) {
    return point.x >= rect.x && point.x <= rect.x + rect.width && point.y >= rect.y &&
           point.y <= rect.y + rect.height;
}
} // namespace

void LogisticsMap::init() {
    data_ = LoadDefaultMapData();
}

void LogisticsMap::draw() const {
    ClearBackground(RAYWHITE);

    drawGrid();
    drawRoads();

    for (std::size_t i = 0; i < data_.warehouses.size(); i++) {
        DrawCircleV(ToRaylib(data_.dockPoints[i]), 5.0f, DARKGRAY);
        drawWarehouse(data_.warehouses[i], static_cast<int>(i));
    }

    drawChargingStation();

    DrawCircleV(ToRaylib(data_.robotStart), 20, ORANGE);
    DrawCircleLines(static_cast<int>(data_.robotStart.x), static_cast<int>(data_.robotStart.y),
                    24.0f, BROWN);
    DrawText("Robot Start", static_cast<int>(data_.robotStart.x) - 47,
             static_cast<int>(data_.robotStart.y) + 34, 18, DARKGRAY);
}

void LogisticsMap::unload() {}

LagerId LogisticsMap::getPickupLagerId() const {
    return static_cast<LagerId>(data_.pickupLagerIndex);
}

LagerId LogisticsMap::getDeliveryLagerId() const {
    return static_cast<LagerId>(data_.deliveryLagerIndex);
}

Vec2 LogisticsMap::getRobotStartPosition() const {
    return data_.robotStart;
}

Vec2 LogisticsMap::getChargingStationPosition() const {
    return {
        data_.chargingStation.body.x + data_.chargingStation.body.width * 0.5f,
        data_.chargingStation.body.y + data_.chargingStation.body.height * 0.5f,
    };
}

Vec2 LogisticsMap::getChargingStationDockPosition() const {
    return data_.chargingStation.dockPoint;
}

std::optional<Vec2> LogisticsMap::getLagerDockPosition(LagerId lagerId) const {
    const std::size_t index = static_cast<std::size_t>(lagerId);
    if (!isValid(lagerId) || index >= data_.dockPoints.size()) {
        return std::nullopt;
    }

    return data_.dockPoints[index];
}

std::optional<Vec2> LogisticsMap::getPickupDockPosition() const {
    return getLagerDockPosition(getPickupLagerId());
}

std::optional<Vec2> LogisticsMap::getDeliveryDockPosition() const {
    return getLagerDockPosition(getDeliveryLagerId());
}

bool LogisticsMap::isRoadPosition(Vec2 position) const {
    for (Rect road : data_.roads) {
        if (ContainsPoint(road, position)) {
            return true;
        }
    }

    return false;
}

Vec2 LogisticsMap::clampPositionToRoad(Vec2 position) const {
    Vec2 closestPosition = position;
    float closestDistanceSquared = 0.0f;
    bool hasClosestPosition = false;

    for (Rect road : data_.roads) {
        const Vec2 clampedPosition = {
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

const std::vector<Vec2>& LogisticsMap::getNavigationNodes() const {
    return data_.navigationNodes;
}

const std::vector<NavigationEdge>& LogisticsMap::getNavigationEdges() const {
    return data_.navigationEdges;
}

const std::vector<BlockingRobotPath>& LogisticsMap::getBlockingRobotPaths() const {
    return data_.blockingRobotPaths;
}

Vec2 LogisticsMap::getWarehouseCenter(std::size_t index) const {
    Rect warehouse = data_.warehouses[index];

    return {
        warehouse.x + warehouse.width * 0.5f,
        warehouse.y + warehouse.height * 0.5f,
    };
}

void LogisticsMap::drawGrid() const {
    for (int x = 0; x <= data_.screenWidth; x += data_.gridSize) {
        DrawLine(x, 0, x, data_.screenHeight, Fade(LIGHTGRAY, 0.55f));
    }

    for (int y = 0; y <= data_.screenHeight; y += data_.gridSize) {
        DrawLine(0, y, data_.screenWidth, y, Fade(LIGHTGRAY, 0.55f));
    }
}

void LogisticsMap::drawRoads() const {
    for (Rect road : data_.roads) {
        DrawRectangleRec(ToRaylib(road), GRAY);
    }

    for (RoadLine roadLine : data_.roadLines) {
        DrawLineEx(ToRaylib(roadLine.start), ToRaylib(roadLine.end), roadLine.width, DARKGRAY);
    }
}

void LogisticsMap::drawWarehouse(Rect body, int index) const {
    const bool isPickupLager = index == data_.pickupLagerIndex;
    const bool isDeliveryLager = index == data_.deliveryLagerIndex;
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

void LogisticsMap::drawChargingStation() const {
    const Rect body = data_.chargingStation.body;
    const Vec2 dock = data_.chargingStation.dockPoint;
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
