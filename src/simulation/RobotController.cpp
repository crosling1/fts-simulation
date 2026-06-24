#include "simulation/RobotController.h"

#include "robots/Robot.h"
#include "robots/WorkerRobot.h"
#include "simulation/BlockingRobotManager.h"
#include "simulation/map.h"

#include <memory>
#include <vector>

namespace {
constexpr float robotSpeed = 120.0f;
constexpr float robotRotationSpeed = 180.0f;
constexpr float robotSize = 16.0f;
constexpr float robotProportionalGain = 2.0f;
constexpr float robotIntegralGain = 0.25f;
constexpr float robotMaxIntegralError = 200.0f;
constexpr float chargeRatePercentagePerSecond = 10.0f;

constexpr Robot::Config robotConfig = {
    {robotSpeed, robotRotationSpeed, robotSize},
    {robotProportionalGain, robotIntegralGain, robotMaxIntegralError},
};
} // namespace

RobotController::RobotController(const LogisticsMap& logisticsMap,
                                 const BlockingRobotManager& blockingRobotManager)
    : logisticsMap_(logisticsMap), blockingRobotManager_(blockingRobotManager),
      routePlanner_(logisticsMap), routeFollower_(logisticsMap), chargingManager_(logisticsMap) {}

void RobotController::initialize() {
    robot_ = std::make_unique<WorkerRobot>(logisticsMap_.getRobotStartPosition(), robotConfig);
    taskFlow_.reset();
    emergencyStopActive_ = false;
    stateBeforeEmergencyStop_ = Robot::State::Idle;
    routeFollower_.setActivePath(
        routePlanner_.buildPathToPickup(logisticsMap_.getRobotStartPosition()),
        logisticsMap_.getRobotStartPosition(), *robot_);
}

void RobotController::update(float deltaTime, const InputState& inputState) {
    if (robot_ == nullptr) {
        return;
    }

    updateEmergencyStop(inputState);
    if (emergencyStopActive_) {
        return;
    }

    const Vector2 robotPosition = robot_->getPosition();

    if (taskFlow_.isPickingUp()) {
        updatePickup(deltaTime);
        return;
    }

    if (taskFlow_.isDroppingOff()) {
        updateDropoff(deltaTime);
        return;
    }

    if (taskFlow_.isCharging()) {
        updateCharging(deltaTime);
        return;
    }

    if (blockingRobotManager_.hasActiveBlockingRobotNear(robotPosition,
                                                         robot_->getProximityDetectionRadius())) {
        return;
    }

    robot_->updateMovement(deltaTime);
    routeFollower_.keepOnRoad(*robot_);
    updateWaypointTravel();
}

void RobotController::draw() const {
    if (robot_ == nullptr) {
        return;
    }

    routeFollower_.draw();

    robot_->drawProximityScanArea();
    robot_->draw();
}

void RobotController::unload() {
    robot_.reset();
}

std::optional<RobotStatusSnapshot> RobotController::statusSnapshot() const {
    if (robot_ == nullptr) {
        return std::nullopt;
    }

    return RobotStatusSnapshot{
        robot_->getState(),
        robot_->getBattery().getChargePercentage(),
        emergencyStopActive_,
    };
}

Vector2 RobotController::getRobotPosition() const {
    return robot_->getPosition();
}

void RobotController::startChargingTrip() {
    const Vector2 startPosition = getRobotPosition();

    taskFlow_.startTripToCharging();
    robot_->setState(Robot::State::Arrived);
    const std::vector<Vector2> chargingPath =
        routePlanner_.buildPathToChargingStation(startPosition);
    routeFollower_.setActivePath(chargingPath, startPosition, *robot_);

    if (chargingPath.empty()) {
        taskFlow_.startCharging();
        robot_->setState(Robot::State::Charging);
    }
}

void RobotController::startPickupTrip() {
    const Vector2 startPosition = getRobotPosition();

    taskFlow_.startTripToPickup();
    robot_->setState(Robot::State::Arrived);
    routeFollower_.setActivePath(routePlanner_.buildPathToPickup(startPosition), startPosition,
                                 *robot_);
}

void RobotController::startDropoffTrip() {
    const Vector2 startPosition = getRobotPosition();

    taskFlow_.startTripToDropoff();
    robot_->setState(Robot::State::CarryingItem);
    routeFollower_.setActivePath(routePlanner_.buildPathToDropoff(startPosition), startPosition,
                                 *robot_);
}

void RobotController::updatePickup(float deltaTime) {
    if (!taskFlow_.updatePickup(deltaTime)) {
        return;
    }

    startDropoffTrip();
}

void RobotController::updateDropoff(float deltaTime) {
    if (!taskFlow_.updateDropoff(deltaTime)) {
        return;
    }

    if (chargingManager_.shouldStartChargingAfterDropoff(*robot_, routePlanner_,
                                                         getRobotPosition())) {
        startChargingTrip();
        return;
    }

    startPickupTrip();
}

void RobotController::updateCharging(float deltaTime) {
    robot_->setState(Robot::State::Charging);
    robot_->getBattery().charge(chargeRatePercentagePerSecond * deltaTime);

    if (!robot_->getBattery().isFull()) {
        return;
    }

    startPickupTrip();
}

void RobotController::updateWaypointTravel() {
    if (!routeFollower_.updateWaypointTravel(*robot_)) {
        return;
    }

    if (taskFlow_.isRoutingToPickup()) {
        taskFlow_.startPickingUp();
        robot_->setState(Robot::State::PickingUp);
        return;
    }

    if (taskFlow_.isRoutingToDropoff()) {
        taskFlow_.startDroppingOff();
        robot_->setState(Robot::State::DroppingOff);
        return;
    }

    if (taskFlow_.isRoutingToCharging()) {
        taskFlow_.startCharging();
        robot_->setState(Robot::State::Charging);
    }
}

void RobotController::updateEmergencyStop(const InputState& inputState) {
    if (inputState.emergencyStopPressed && !emergencyStopActive_) {
        emergencyStopActive_ = true;
        stateBeforeEmergencyStop_ = robot_->getState();
        robot_->setState(Robot::State::Idle);
    }

    if (inputState.resetEmergencyStopPressed && emergencyStopActive_) {
        emergencyStopActive_ = false;
        robot_->setState(stateBeforeEmergencyStop_);
    }
}
