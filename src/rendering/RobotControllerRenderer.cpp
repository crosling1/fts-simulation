#include "simulation/RobotController.h"

void RobotController::draw(const IRobotRenderer& robotRenderer) const {
    if (robot_ == nullptr) {
        return;
    }

    routeFollower_.draw();

    robotRenderer.draw(robot_->renderData());
}
