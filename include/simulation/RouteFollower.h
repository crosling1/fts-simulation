#pragma once

#include "simulation/Geometry.h"

#include <cstddef>
#include <vector>

class ILogisticsMap;
class Robot;

class RouteFollower {
  public:
    explicit RouteFollower(const ILogisticsMap& logisticsMap);

    void reset();
    void setActivePath(const std::vector<Vec2>& path, Vec2 pathStart, Robot& robot);
    void keepOnRoad(Robot& robot) const;
    bool updateWaypointTravel(Robot& robot);
    [[nodiscard]] const std::vector<Vec2>& getActivePath() const noexcept;
    [[nodiscard]] Vec2 getActivePathStart() const noexcept;

  private:
    bool setNextWaypoint(Robot& robot);

    const ILogisticsMap& logisticsMap_;
    std::vector<Vec2> activePath_;
    Vec2 activePathStart_ = {0.0f, 0.0f};
    std::size_t currentWaypointIndex_ = 0;
};
