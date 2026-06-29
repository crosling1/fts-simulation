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
    struct AdjacencyEntry {
        int targetNodeIndex;
        float weight;
    };

    [[nodiscard]] static std::vector<std::vector<bool>>
    buildRoadSegmentValidity(const ILogisticsMap& logisticsMap);
    [[nodiscard]] static std::vector<std::vector<AdjacencyEntry>>
    buildAdjacencyList(const ILogisticsMap& logisticsMap,
                       const std::vector<std::vector<bool>>& validRoadSegments);

    const ILogisticsMap& logisticsMap_;
    // Road-segment validity and weighted adjacency are built once during construction
    // and reused by every path query.
    std::vector<std::vector<bool>> validRoadSegments_;
    std::vector<std::vector<AdjacencyEntry>> adjacency_;
};
