#ifndef NAVIGATION_H
#define NAVIGATION_H

#include "raylib.h"

#include <vector>

class LogisticsMap;

std::vector<Vector2>
FindNavigationPath(const LogisticsMap& logisticsMap, Vector2 start,
                   Vector2 goal); // NOLINT(bugprone-easily-swappable-parameters)

#endif // NAVIGATION_H
