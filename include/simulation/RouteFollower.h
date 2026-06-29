#pragma once

#include "raylib.h"

#include <cstddef>
#include <vector>

class ILogisticsMap;
class Robot;

class RouteFollower {
  public:
    explicit RouteFollower(const ILogisticsMap& logisticsMap);

    void reset();
    void setActivePath(const std::vector<Vector2>& path, Vector2 pathStart, Robot& robot);
    void keepOnRoad(Robot& robot) const;
    bool updateWaypointTravel(Robot& robot);
    void draw() const;

  private:
    bool setNextWaypoint(Robot& robot);

    const ILogisticsMap& logisticsMap_;
    std::vector<Vector2> activePath_;
    Vector2 activePathStart_ = {0.0f, 0.0f};
    std::size_t currentWaypointIndex_ = 0;
};
