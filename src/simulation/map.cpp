#include "simulation/map.h"

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
