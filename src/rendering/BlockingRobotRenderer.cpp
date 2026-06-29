#include "rendering/SimulationRenderer.h"

#include "rendering/RaylibGeometry.h"
#include "simulation/BlockingRobotManager.h"

void DrawBlockingRobots(const BlockingRobotManager& blockingRobotManager) {
    for (const BlockingRobot& blockingRobot : blockingRobotManager.getBlockingRobots()) {
        const Vec2 position = blockingRobot.position;
        const float radius = blockingRobot.radius;

        DrawCircleV(ToRaylib(position), radius, PURPLE);
        DrawCircleLines(static_cast<int>(position.x), static_cast<int>(position.y), radius,
                        DARKPURPLE);
        DrawText("B", static_cast<int>(position.x) - 5, static_cast<int>(position.y) - 10, 20,
                 WHITE);
    }
}
