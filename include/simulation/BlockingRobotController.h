#ifndef BLOCKING_ROBOT_CONTROLLER_H
#define BLOCKING_ROBOT_CONTROLLER_H

#include "raylib.h"

#ifdef __cplusplus
extern "C" {
#endif

void InitBlockingRobotController(void);
void UpdateBlockingRobotController(void);
void DrawBlockingRobotController(void);
void UnloadBlockingRobotController(void);
bool HasBlockingRobotNear(Vector2 position, float detectionRadius);

#ifdef __cplusplus
}
#endif

#endif // BLOCKING_ROBOT_CONTROLLER_H
