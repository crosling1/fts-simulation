#include "simulation/BlockingRobotManager.h"

#include "rendering/RaylibGeometry.h"

void BlockingRobotManager::draw() const {
    for (const BlockingRobot& blockingRobot : getBlockingRobots()) {
        const Vec2 position = blockingRobot.position;
        const float radius = blockingRobot.radius;

        DrawCircleV(ToRaylib(position), radius, PURPLE);
        DrawCircleLines(static_cast<int>(position.x), static_cast<int>(position.y), radius,
                        DARKPURPLE);
        DrawText("B", static_cast<int>(position.x) - 5, static_cast<int>(position.y) - 10, 20,
                 WHITE);
    }
}
