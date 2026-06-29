#pragma once

#include "robots/Robot.h"
#include <string_view>

class WorkerRobot : public Robot {
  public:
    WorkerRobot(Pose startPose, Config config, const SimConfig& simConfig = SimConfig::Default());
    WorkerRobot(const Vec2& startPosition, Config config,
                const SimConfig& simConfig = SimConfig::Default());

    [[nodiscard]] std::string_view typeName() const noexcept override;
};
