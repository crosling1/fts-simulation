#ifndef BLOCKING_ROBOT_CONTROLLER_H
#define BLOCKING_ROBOT_CONTROLLER_H

#include "raylib.h"

class LogisticsMap;

void InitBlockingRobotController(const LogisticsMap& logisticsMap);
void UpdateBlockingRobotController(void);
void DrawBlockingRobotController(void);
void UnloadBlockingRobotController(void);
bool HasBlockingRobotNear(Vector2 position, float detectionRadius);

#endif // BLOCKING_ROBOT_CONTROLLER_H
