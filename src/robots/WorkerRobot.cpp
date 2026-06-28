#include "robots/WorkerRobot.h"

#include <string_view>

WorkerRobot::WorkerRobot(Pose startPose, Config config, SimConfig simConfig)
    : Robot(startPose, config, simConfig) {}

WorkerRobot::WorkerRobot(const Vector2& startPosition, Config config, SimConfig simConfig)
    : Robot(startPosition, config, simConfig) {}

std::string_view WorkerRobot::typeName() const noexcept {
    return "WorkerRobot";
}
