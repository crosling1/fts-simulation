#pragma once

#include "raylib.h"

#include <vector>

class ILogisticsMap;

class NavigationGraph {
  public:
    explicit NavigationGraph(const ILogisticsMap& logisticsMap);

    [[nodiscard]] std::vector<Vector2>
    findPath(Vector2 start, Vector2 goal) const; // NOLINT(bugprone-easily-swappable-parameters)

  private:
    const ILogisticsMap& logisticsMap_;
    std::vector<std::vector<int>> adjacency_;
};
