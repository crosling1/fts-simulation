#ifndef ROBOT_CONTROLLER_H
#define ROBOT_CONTROLLER_H

#include "simulation/map.h"

#ifdef __cplusplus
extern "C" {
#endif

void InitRobotController(void);
void UpdateRobotController(void);
void DrawRobotController(void);
void UnloadRobotController(void);

#ifdef __cplusplus
}
#endif

#endif // ROBOT_CONTROLLER_H
