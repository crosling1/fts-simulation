#include "simulation/RobotRoutePlanner.h"

#include "simulation/ILogisticsMap.h"

#include <cmath>

namespace {
float Distance(Vector2 from, Vector2 to) {
    const float deltaX = to.x - from.x;
    const float deltaY = to.y - from.y;

    return std::sqrt((deltaX * deltaX) + (deltaY * deltaY));
}
} // namespace

RobotRoutePlanner::RobotRoutePlanner(const ILogisticsMap& logisticsMap)
    : logisticsMap_(logisticsMap), navigationGraph_(logisticsMap) {}

std::vector<Vector2> RobotRoutePlanner::buildPathToPickup(Vector2 startPosition) const {
    const auto pickupDock = logisticsMap_.getPickupDockPosition();
    if (!pickupDock) {
        return {};
    }

    return navigationGraph_.findPath(startPosition, *pickupDock);
}

std::vector<Vector2> RobotRoutePlanner::buildPathToDropoff(Vector2 startPosition) const {
    const auto deliveryDock = logisticsMap_.getDeliveryDockPosition();
    if (!deliveryDock) {
        return {};
    }

    return navigationGraph_.findPath(startPosition, *deliveryDock);
}

std::vector<Vector2> RobotRoutePlanner::buildPathToChargingStation(Vector2 startPosition) const {
    return navigationGraph_.findPath(startPosition, logisticsMap_.getChargingStationDockPosition());
}

float RobotRoutePlanner::calculatePathDistance(Vector2 startPosition,
                                               const std::vector<Vector2>& path) const {
    float distance = 0.0f;
    Vector2 previousPoint = startPosition;

    for (Vector2 waypoint : path) {
        distance += Distance(previousPoint, waypoint);
        previousPoint = waypoint;
    }

    return distance;
}
