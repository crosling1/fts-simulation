#ifndef ROBOT_CONTROLLER_H
#define ROBOT_CONTROLLER_H

#include "simulation/map.h"
#include "simulation/RobotStatusSnapshot.h"

#include <optional>

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

std::optional<RobotStatusSnapshot> GetRobotStatusSnapshot(void);

#endif // ROBOT_CONTROLLER_H
