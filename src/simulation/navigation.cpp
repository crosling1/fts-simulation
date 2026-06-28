#include "simulation/navigation.h"

#include "simulation/NavigationGraph.h"

std::vector<Vector2>
FindNavigationPath(const ILogisticsMap& logisticsMap, Vector2 start,
                   Vector2 goal) { // NOLINT(bugprone-easily-swappable-parameters)
    return NavigationGraph(logisticsMap).findPath(start, goal);
}
