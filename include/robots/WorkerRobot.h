#pragma once

#include "robots/Robot.h"
#include <string_view>

class WorkerRobot : public Robot {
  public:
    WorkerRobot(Pose startPose, Config config, SimConfig simConfig = SimConfig::Default());
    WorkerRobot(const Vector2& startPosition, Config config,
                SimConfig simConfig = SimConfig::Default());

    [[nodiscard]] std::string_view typeName() const noexcept override;
};
