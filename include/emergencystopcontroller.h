#pragma once

#include "robots/Robot.h"
#include "simulation/InputState.h"

#include "raylib.h"

class BlockingRobotManager;

class EmergencyStopController {
  public:
    explicit EmergencyStopController(const BlockingRobotManager& blockingRobotManager);

    void reset();
    void updateEmergencyStop(const InputState& inputState, Robot& robot);
    bool isEmergencyStopActive() const;
    bool shouldPauseForObstacle(Vector2 robotPosition, float proximityDetectionRadius) const;

  private:
    const BlockingRobotManager& blockingRobotManager_;
    bool emergencyStopActive_ = false;
    Robot::State stateBeforeEmergencyStop_ = Robot::State::Idle;
};
