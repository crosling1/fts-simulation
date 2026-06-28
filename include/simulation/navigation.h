#pragma once

#include "raylib.h"

#include <vector>

class ILogisticsMap;

[[nodiscard]] std::vector<Vector2>
FindNavigationPath(const ILogisticsMap& logisticsMap, Vector2 start,
                   Vector2 goal); // NOLINT(bugprone-easily-swappable-parameters)
