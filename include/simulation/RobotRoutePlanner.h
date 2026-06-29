#pragma once

#include "simulation/NavigationGraph.h"
#include "simulation/Geometry.h"

#include <vector>

class ILogisticsMap;

class RobotRoutePlanner {
  public:
    explicit RobotRoutePlanner(const ILogisticsMap& logisticsMap);

    [[nodiscard]] std::vector<Vec2> buildPathToPickup(Vec2 startPosition) const;
    [[nodiscard]] std::vector<Vec2> buildPathToDropoff(Vec2 startPosition) const;
    [[nodiscard]] std::vector<Vec2> buildPathToChargingStation(Vec2 startPosition) const;
    [[nodiscard]] float calculatePathDistance(Vec2 startPosition,
                                              const std::vector<Vec2>& path) const;

  private:
    const ILogisticsMap& logisticsMap_;
    NavigationGraph navigationGraph_;
};
