#include "navigation.h"

#include "map.h"

#include <algorithm>
#include <array>
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
constexpr int pathEdgeCount = 30;

struct PathEdge {
    int from;
    int to;
};

struct PathQueueEntry {
    int nodeIndex;
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

float DistanceSquared(Vector2 from, Vector2 to) {
    const float deltaX = to.x - from.x;
    const float deltaY = to.y - from.y;

    return (deltaX * deltaX) + (deltaY * deltaY);
}

float Distance(Vector2 from, Vector2 to) {
    return std::sqrt(DistanceSquared(from, to));
}

bool IsRoadSample(Vector2 position) {
    if (IsMapRoadPosition(position)) {
        return true;
    }

    return DistanceSquared(position, ClampPositionToMapRoad(position)) <=
           (roadSampleTolerance * roadSampleTolerance);
}

bool IsRoadSegment(Vector2 from, Vector2 to) { // NOLINT(bugprone-easily-swappable-parameters)
    const float segmentLength = Distance(from, to);
    if (segmentLength <= roadSampleTolerance) {
        return IsRoadSample(from) && IsRoadSample(to);
    }

    const int sampleCount = static_cast<int>(std::ceil(segmentLength / roadSampleSpacing));
    for (int i = 0; i <= sampleCount; i++) {
        const float t = static_cast<float>(i) / static_cast<float>(sampleCount);
        const Vector2 sample = {
            from.x + ((to.x - from.x) * t),
            from.y + ((to.y - from.y) * t),
        };

        if (!IsRoadSample(sample)) {
            return false;
        }
    }

    return true;
}

std::vector<Vector2> BuildPathNodes(void) {
    return {
        GetRobotStartPosition(),
        {200.0f, 620.0f},
        {200.0f, 450.0f},
        {200.0f, 150.0f},
        {540.0f, 450.0f},
        {800.0f, 450.0f},
        {960.0f, 450.0f},
        {1040.0f, 450.0f},
        {800.0f, 470.0f},
        {800.0f, 350.0f},
        {800.0f, 270.0f},
        {800.0f, 150.0f},
        {540.0f, 150.0f},
        {540.0f, 270.0f},
        {540.0f, 350.0f},
        {960.0f, 350.0f},
        {960.0f, 556.0f},
        GetLagerDockPosition(LAGER_5),
        {340.0f, 270.0f},
        GetLagerDockPosition(LAGER_1),
        {450.0f, 270.0f},
        GetLagerDockPosition(LAGER_2),
        {675.0f, 270.0f},
        GetLagerDockPosition(LAGER_3),
        GetLagerDockPosition(LAGER_4),
        {545.0f, 450.0f},
        GetLagerDockPosition(LAGER_6),
        {800.0f, 257.0f},
    };
}

std::array<PathEdge, pathEdgeCount> BuildPathEdges(void) {
    return {{
        {0, 1},   {1, 2},   {2, 3},   {2, 4},   {4, 25},  {25, 5},  {5, 6},   {6, 7},
        {5, 8},   {5, 9},   {9, 10},  {10, 11}, {12, 13}, {13, 14}, {14, 9},  {9, 15},
        {15, 6},  {15, 16}, {16, 17}, {18, 19}, {18, 20}, {20, 13}, {13, 22}, {22, 10},
        {20, 21}, {22, 23}, {10, 27}, {27, 24}, {25, 26}, {14, 15},
    }};
}

int FindNearestPathNode(const std::vector<Vector2>& pathNodes, Vector2 position) {
    int nearestNodeIndex = invalidPathNode;
    float nearestDistanceSquared = unreachableDistance;

    for (std::size_t i = 0; i < pathNodes.size(); i++) {
        const float candidateDistanceSquared = DistanceSquared(position, pathNodes[i]);
        if (candidateDistanceSquared < nearestDistanceSquared) {
            nearestDistanceSquared = candidateDistanceSquared;
            nearestNodeIndex = static_cast<int>(i);
        }
    }

    return nearestNodeIndex;
}

int GetNeighborNodeIndex(PathEdge edge, int nodeIndex) {
    if (edge.from == nodeIndex) {
        return edge.to;
    }

    if (edge.to == nodeIndex) {
        return edge.from;
    }

    return invalidPathNode;
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
} // namespace

std::vector<Vector2>
FindNavigationPath(Vector2 start, Vector2 goal) { // NOLINT(bugprone-easily-swappable-parameters)
    const std::vector<Vector2> pathNodes = BuildPathNodes();
    const std::array<PathEdge, pathEdgeCount> pathEdges = BuildPathEdges();
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
    std::vector<bool> closed(pathNodes.size(), false);
    std::priority_queue<PathQueueEntry, std::vector<PathQueueEntry>, ComparePathQueueEntry>
        openNodes;

    costFromStart[endpoints.startNodeIndex] = 0.0f;
    openNodes.push({
        endpoints.startNodeIndex,
        Distance(pathNodes[endpoints.startNodeIndex], pathNodes[endpoints.goalNodeIndex]),
    });

    while (!openNodes.empty()) {
        const int currentNodeIndex = openNodes.top().nodeIndex;
        openNodes.pop();

        if (closed[currentNodeIndex]) {
            continue;
        }

        if (currentNodeIndex == endpoints.goalNodeIndex) {
            return BuildPathFromParents(pathNodes, cameFrom, endpoints);
        }

        closed[currentNodeIndex] = true;
        for (PathEdge edge : pathEdges) {
            const int neighborNodeIndex = GetNeighborNodeIndex(edge, currentNodeIndex);
            if (neighborNodeIndex == invalidPathNode || closed[neighborNodeIndex]) {
                continue;
            }

            if (!IsRoadSegment(pathNodes[currentNodeIndex], pathNodes[neighborNodeIndex])) {
                continue;
            }

            const float newCost =
                costFromStart[currentNodeIndex] +
                Distance(pathNodes[currentNodeIndex], pathNodes[neighborNodeIndex]);
            if (newCost >= costFromStart[neighborNodeIndex]) {
                continue;
            }

            costFromStart[neighborNodeIndex] = newCost;
            cameFrom[neighborNodeIndex] = currentNodeIndex;
            openNodes.push({
                neighborNodeIndex,
                newCost +
                    Distance(pathNodes[neighborNodeIndex], pathNodes[endpoints.goalNodeIndex]),
            });
        }
    }

    return {};
}
