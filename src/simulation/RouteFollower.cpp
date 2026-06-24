#include "simulation/RouteFollower.h"

#include "robots/Robot.h"
#include "simulation/map.h"

RouteFollower::RouteFollower(const LogisticsMap& logisticsMap) : logisticsMap_(logisticsMap) {}

void RouteFollower::setActivePath(const std::vector<Vector2>& path, Vector2 pathStart,
                                  Robot& robot) {
    activePath_ = path;
    activePathStart_ = pathStart;
    currentWaypointIndex_ = 0;
    setNextWaypoint(robot);
}

void RouteFollower::keepOnRoad(Robot& robot) const {
    const Vector2 robotPosition = robot.getPosition();
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

void RouteFollower::draw() const {
    Vector2 previousPoint = activePathStart_;
    for (Vector2 waypoint : activePath_) {
        DrawLineEx(previousPoint, waypoint, 3.0f, MAGENTA);
        DrawCircleV(waypoint, 5.0f, MAGENTA);
        previousPoint = waypoint;
    }
}

bool RouteFollower::setNextWaypoint(Robot& robot) {
    if (currentWaypointIndex_ >= activePath_.size()) {
        return false;
    }

    robot.setTargetPosition(activePath_[currentWaypointIndex_]);
    currentWaypointIndex_++;
    return true;
}
