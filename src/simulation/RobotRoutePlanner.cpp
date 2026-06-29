#include "simulation/RobotRoutePlanner.h"

#include "simulation/ILogisticsMap.h"
#include "simulation/MathUtils.h"

RobotRoutePlanner::RobotRoutePlanner(const ILogisticsMap& logisticsMap)
    : logisticsMap_(logisticsMap), navigationGraph_(logisticsMap) {}

std::vector<Vec2> RobotRoutePlanner::buildPathToPickup(Vec2 startPosition) const {
    const auto pickupDock = logisticsMap_.getPickupDockPosition();
    if (!pickupDock) {
        return {};
    }

    return navigationGraph_.findPath(startPosition, *pickupDock);
}

std::vector<Vec2> RobotRoutePlanner::buildPathToDropoff(Vec2 startPosition) const {
    const auto deliveryDock = logisticsMap_.getDeliveryDockPosition();
    if (!deliveryDock) {
        return {};
    }

    return navigationGraph_.findPath(startPosition, *deliveryDock);
}

std::vector<Vec2> RobotRoutePlanner::buildPathToChargingStation(Vec2 startPosition) const {
    return navigationGraph_.findPath(startPosition, logisticsMap_.getChargingStationDockPosition());
}

float RobotRoutePlanner::calculatePathDistance(Vec2 startPosition,
                                               const std::vector<Vec2>& path) const {
    float distance = 0.0f;
    Vec2 previousPoint = startPosition;

    for (Vec2 waypoint : path) {
        distance += math::distance(previousPoint, waypoint);
        previousPoint = waypoint;
    }

    return distance;
}
