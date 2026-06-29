#include "simulation/RouteFollower.h"

#include "robots/Robot.h"
#include "simulation/ILogisticsMap.h"

RouteFollower::RouteFollower(const ILogisticsMap& logisticsMap) : logisticsMap_(logisticsMap) {}

void RouteFollower::reset() {
    activePath_.clear();
    activePathStart_ = {0.0f, 0.0f};
    currentWaypointIndex_ = 0;
}

void RouteFollower::setActivePath(const std::vector<Vec2>& path, Vec2 pathStart, Robot& robot) {
    activePath_ = path;
    activePathStart_ = pathStart;
    currentWaypointIndex_ = 0;
    setNextWaypoint(robot);
}

void RouteFollower::keepOnRoad(Robot& robot) const {
    const Vec2 robotPosition = robot.getPosition();
    if (!logisticsMap_.isRoadPosition(robotPosition)) {
        robot.setPosition(logisticsMap_.clampPositionToRoad(robotPosition));
    }
}

bool RouteFollower::updateWaypointTravel(Robot& robot) {
    if (!robot.hasReachedTarget()) {
        return false;
    }

    return !setNextWaypoint(robot);
}

bool RouteFollower::setNextWaypoint(Robot& robot) {
    if (currentWaypointIndex_ >= activePath_.size()) {
        return false;
    }

    robot.setTargetPosition(activePath_[currentWaypointIndex_]);
    currentWaypointIndex_++;
    return true;
}
