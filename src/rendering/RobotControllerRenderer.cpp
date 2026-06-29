#include "rendering/SimulationRenderer.h"

#include "simulation/RobotController.h"

void DrawRobotController(const RobotController& robotController,
                         const IRobotRenderer& robotRenderer) {
    const std::optional<RobotRenderData> renderData = robotController.robotRenderData();
    if (!renderData) {
        return;
    }

    DrawRouteFollower(robotController.routeFollower());

    robotRenderer.draw(*renderData);
}
