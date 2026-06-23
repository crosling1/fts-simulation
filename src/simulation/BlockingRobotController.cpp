#include "simulation/BlockingRobotController.h"

#include "simulation/BlockingRobotManager.h"

namespace {
BlockingRobotManager blockingRobotManager;
} // namespace

void InitBlockingRobotController(const LogisticsMap& logisticsMap) {
    blockingRobotManager.initBlockingRobots(logisticsMap);
}

void UpdateBlockingRobotController(void) {
    blockingRobotManager.update(GetFrameTime());
}

void DrawBlockingRobotController(void) {
    blockingRobotManager.draw();
}

void UnloadBlockingRobotController(void) {
    blockingRobotManager.clear();
}

bool HasBlockingRobotNear(Vector2 position, float detectionRadius) {
    return blockingRobotManager.hasActiveBlockingRobotNear(position, detectionRadius);
}
