#pragma once

#include "raylib.h"

#include <vector>

class LogisticsMap;

std::vector<Vector2>
FindNavigationPath(const LogisticsMap& logisticsMap, Vector2 start,
                   Vector2 goal); // NOLINT(bugprone-easily-swappable-parameters)
