#include "simulation/BlockingRobotController.h"

#include "simulation/ObstacleManager.h"

namespace {
ObstacleManager obstacleManager;
} // namespace

void InitBlockingRobotController(void) {
    obstacleManager.initBlockingRobots();
}

void UpdateBlockingRobotController(void) {
    obstacleManager.update(GetFrameTime());
}

void DrawBlockingRobotController(void) {
    obstacleManager.draw();
}

void UnloadBlockingRobotController(void) {
    obstacleManager.clear();
}

bool HasBlockingRobotNear(Vector2 position, float detectionRadius) {
    return obstacleManager.hasActiveObstacleNear(position, detectionRadius);
}
