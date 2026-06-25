#pragma once

#include "robots/Robot.h"
#include <string_view>

class WorkerRobot : public Robot {
  public:
    WorkerRobot(Pose startPose, Config config);
    WorkerRobot(const Vector2& startPosition, Config config);

    [[nodiscard]] std::string_view typeName() const noexcept override;
};
