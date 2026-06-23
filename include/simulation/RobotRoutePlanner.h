#ifndef ROBOT_ROUTE_PLANNER_H
#define ROBOT_ROUTE_PLANNER_H

#include "raylib.h"

#include <vector>

class LogisticsMap;

class RobotRoutePlanner {
  public:
    explicit RobotRoutePlanner(const LogisticsMap& logisticsMap);

    std::vector<Vector2> buildPathToPickup(Vector2 startPosition) const;
    std::vector<Vector2> buildPathToDropoff(Vector2 startPosition) const;
    std::vector<Vector2> buildPathToChargingStation(Vector2 startPosition) const;
    float calculatePathDistance(Vector2 startPosition, const std::vector<Vector2>& path) const;

  private:
    const LogisticsMap& logisticsMap_;
};

#endif // ROBOT_ROUTE_PLANNER_H
