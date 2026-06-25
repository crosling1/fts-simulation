#include "robots/WorkerRobot.h"

#include <string_view>

WorkerRobot::WorkerRobot(Pose startPose, Config config) : Robot(startPose, config) {}

WorkerRobot::WorkerRobot(const Vector2& startPosition, Config config)
    : Robot(startPosition, config) {}

std::string_view WorkerRobot::typeName() const noexcept {
    return "WorkerRobot";
}
