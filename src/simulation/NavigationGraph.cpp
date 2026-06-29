#include "simulation/NavigationGraph.h"

#include "simulation/ILogisticsMap.h"
#include "simulation/MathUtils.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <queue>
#include <vector>

namespace {
constexpr float unreachableDistance = std::numeric_limits<float>::max();
constexpr float roadSampleSpacing = 8.0f;
constexpr float roadSampleTolerance = 1.0f;
constexpr int invalidPathNode = -1;

struct PathQueueEntry {
    int nodeIndex;
    float costFromStart;
    float priority;
};

struct PathEndpointIndices {
    int startNodeIndex;
    int goalNodeIndex;
};

struct ComparePathQueueEntry {
    bool operator()(PathQueueEntry left, PathQueueEntry right) const {
        return left.priority > right.priority;
    }
};

bool IsRoadSample(const ILogisticsMap& logisticsMap, Vector2 position) {
    if (logisticsMap.isRoadPosition(position)) {
        return true;
    }

    return math::distanceSq(position, logisticsMap.clampPositionToRoad(position)) <=
           (roadSampleTolerance * roadSampleTolerance);
}

bool IsRoadSegment(const ILogisticsMap& logisticsMap, Vector2 from,
                   Vector2 to) { // NOLINT(bugprone-easily-swappable-parameters)
    const float segmentLength = math::distance(from, to);
    if (segmentLength <= roadSampleTolerance) {
        return IsRoadSample(logisticsMap, from) && IsRoadSample(logisticsMap, to);
    }

    const int sampleCount = static_cast<int>(std::ceil(segmentLength / roadSampleSpacing));
    for (int i = 0; i <= sampleCount; i++) {
        const float t = static_cast<float>(i) / static_cast<float>(sampleCount);
        const Vector2 sample = {
            from.x + ((to.x - from.x) * t),
            from.y + ((to.y - from.y) * t),
        };

        if (!IsRoadSample(logisticsMap, sample)) {
            return false;
        }
    }

    return true;
}

int FindNearestPathNode(const std::vector<Vector2>& pathNodes, Vector2 position) {
    int nearestNodeIndex = invalidPathNode;
    float nearestDistanceSquared = unreachableDistance;

    for (std::size_t i = 0; i < pathNodes.size(); i++) {
        const float candidateDistanceSquared = math::distanceSq(position, pathNodes[i]);
        if (candidateDistanceSquared < nearestDistanceSquared) {
            nearestDistanceSquared = candidateDistanceSquared;
            nearestNodeIndex = static_cast<int>(i);
        }
    }

    return nearestNodeIndex;
}

std::vector<Vector2> BuildPathFromParents(const std::vector<Vector2>& pathNodes,
                                          const std::vector<int>& cameFrom,
                                          PathEndpointIndices endpoints) {
    std::vector<Vector2> path;
    int currentNodeIndex = endpoints.goalNodeIndex;

    while (currentNodeIndex != invalidPathNode && currentNodeIndex != endpoints.startNodeIndex) {
        path.push_back(pathNodes[currentNodeIndex]);
        currentNodeIndex = cameFrom[currentNodeIndex];
    }

    if (currentNodeIndex != endpoints.startNodeIndex) {
        return {};
    }

    std::reverse(path.begin(), path.end());
    return path;
}

std::vector<Vector2> SimplifyPath(const ILogisticsMap& logisticsMap,
                                  const std::vector<Vector2>& path) {
    if (path.size() < 3) {
        return path;
    }

    std::vector<Vector2> simplifiedPath;
    simplifiedPath.push_back(path.front());

    for (std::size_t i = 1; i + 1 < path.size(); i++) {
        if (IsRoadSegment(logisticsMap, simplifiedPath.back(), path[i + 1])) {
            continue;
        }

        simplifiedPath.push_back(path[i]);
    }

    simplifiedPath.push_back(path.back());
    return simplifiedPath;
}

std::vector<std::vector<int>> BuildAdjacencyList(const std::vector<NavigationEdge>& edges,
                                                 std::size_t nodeCount) {

    std::vector<std::vector<int>> adj(nodeCount);
    for (const NavigationEdge& edge : edges) {
        adj[edge.from].push_back(static_cast<int>(edge.to));
        adj[edge.to].push_back(static_cast<int>(edge.from));
    }
    return adj;
}
} // namespace

NavigationGraph::NavigationGraph(const ILogisticsMap& logisticsMap)
    : logisticsMap_(logisticsMap),
      adjacency_(BuildAdjacencyList(logisticsMap.getNavigationEdges(),
                                    logisticsMap.getNavigationNodes().size())) {}

std::vector<Vector2>
NavigationGraph::findPath(Vector2 start,
                          Vector2 goal) const { // NOLINT(bugprone-easily-swappable-parameters)
    const std::vector<Vector2>& pathNodes = logisticsMap_.getNavigationNodes();
    const PathEndpointIndices endpoints = {
        FindNearestPathNode(pathNodes, start),
        FindNearestPathNode(pathNodes, goal),
    };

    if (endpoints.startNodeIndex < 0 || endpoints.goalNodeIndex < 0) {
        return {};
    }

    if (endpoints.startNodeIndex == endpoints.goalNodeIndex) {
        return {};
    }

    std::vector<float> costFromStart(pathNodes.size(), unreachableDistance);
    std::vector<int> cameFrom(pathNodes.size(), invalidPathNode);
    std::priority_queue<PathQueueEntry, std::vector<PathQueueEntry>, ComparePathQueueEntry>
        openNodes;

    costFromStart[endpoints.startNodeIndex] = 0.0f;
    openNodes.push({
        endpoints.startNodeIndex,
        0.0f,
        math::distance(pathNodes[endpoints.startNodeIndex], pathNodes[endpoints.goalNodeIndex]),
    });

    while (!openNodes.empty()) {
        const PathQueueEntry currentEntry = openNodes.top();
        openNodes.pop();

        const int currentNodeIndex = currentEntry.nodeIndex;

        if (currentEntry.costFromStart > costFromStart[currentNodeIndex]) {
            continue;
        }

        if (currentNodeIndex == endpoints.goalNodeIndex) {
            return SimplifyPath(logisticsMap_,
                                BuildPathFromParents(pathNodes, cameFrom, endpoints));
        }

        for (int neighborNodeIndex : adjacency_[currentNodeIndex]) {
            if (!IsRoadSegment(logisticsMap_, pathNodes[currentNodeIndex],
                               pathNodes[neighborNodeIndex])) {
                continue;
            }

            const float newCost =
                costFromStart[currentNodeIndex] +
                math::distance(pathNodes[currentNodeIndex], pathNodes[neighborNodeIndex]);

            if (newCost >= costFromStart[neighborNodeIndex]) {
                continue;
            }

            costFromStart[neighborNodeIndex] = newCost;
            cameFrom[neighborNodeIndex] = currentNodeIndex;

            openNodes.push({
                neighborNodeIndex,
                newCost,
                newCost + math::distance(pathNodes[neighborNodeIndex],
                                         pathNodes[endpoints.goalNodeIndex]),
            });
        }
    }

    return {};
}
