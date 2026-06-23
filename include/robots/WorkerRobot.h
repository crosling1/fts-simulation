#pragma once

#include "robots/Robot.h"

class WorkerRobot : public Robot {
  public:
    WorkerRobot(Pose startPose, Config config);
    WorkerRobot(const Vector2& startPosition, Config config);

    void printType() const override;
};
