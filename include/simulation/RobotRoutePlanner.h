#pragma once

#include "simulation/NavigationGraph.h"
#include "raylib.h"

#include <vector>

class ILogisticsMap;

class RobotRoutePlanner {
  public:
    explicit RobotRoutePlanner(const ILogisticsMap& logisticsMap);

    [[nodiscard]] std::vector<Vector2> buildPathToPickup(Vector2 startPosition) const;
    [[nodiscard]] std::vector<Vector2> buildPathToDropoff(Vector2 startPosition) const;
    [[nodiscard]] std::vector<Vector2> buildPathToChargingStation(Vector2 startPosition) const;
    [[nodiscard]] float calculatePathDistance(Vector2 startPosition,
                                              const std::vector<Vector2>& path) const;

  private:
    const ILogisticsMap& logisticsMap_;
    NavigationGraph navigationGraph_;
};
