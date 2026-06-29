#include "simulation/EmergencyStopController.h"

#include "simulation/BlockingRobotManager.h"

namespace {
Robot::State RestoredStateAfterEmergencyStop(Robot::State stateBeforeEmergencyStop) {
    if (stateBeforeEmergencyStop == Robot::State::BatteryDepleted) {
        return Robot::State::Idle;
    }

    return stateBeforeEmergencyStop;
}
} // namespace

EmergencyStopController::EmergencyStopController(const BlockingRobotManager& blockingRobotManager)
    : blockingRobotManager_(blockingRobotManager) {}

void EmergencyStopController::reset() {
    emergencyStopActive_ = false;
    stateBeforeEmergencyStop_ = Robot::State::Idle;
}

void EmergencyStopController::updateEmergencyStop(const InputState& inputState, Robot& robot) {
    if (inputState.emergencyStopPressed && !emergencyStopActive_) {
        emergencyStopActive_ = true;
        stateBeforeEmergencyStop_ = robot.getState();
        robot.setState(Robot::State::Idle);
    }

    if (inputState.resetEmergencyStopPressed && emergencyStopActive_) {
        emergencyStopActive_ = false;
        robot.setState(RestoredStateAfterEmergencyStop(stateBeforeEmergencyStop_));
    }
}

bool EmergencyStopController::isEmergencyStopActive() const {
    return emergencyStopActive_;
}

bool EmergencyStopController::shouldPauseForObstacle(Vec2 robotPosition,
                                                     float proximityDetectionRadius) const {
    return blockingRobotManager_.hasActiveBlockingRobotNear(robotPosition,
                                                            proximityDetectionRadius);
}
