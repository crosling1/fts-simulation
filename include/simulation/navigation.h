#pragma once

#include "simulation/Geometry.h"

#include <vector>

class ILogisticsMap;

[[nodiscard]] std::vector<Vec2>
FindNavigationPath(const ILogisticsMap& logisticsMap, Vec2 start,
                   Vec2 goal); // NOLINT(bugprone-easily-swappable-parameters)
