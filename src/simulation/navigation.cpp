#include "simulation/navigation.h"

#include "simulation/NavigationGraph.h"

std::vector<Vec2> FindNavigationPath(const ILogisticsMap& logisticsMap, Vec2 start,
                                     Vec2 goal) { // NOLINT(bugprone-easily-swappable-parameters)
    return NavigationGraph(logisticsMap).findPath(start, goal);
}
