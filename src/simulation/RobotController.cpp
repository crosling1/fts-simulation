#include "simulation/RobotController.h"
#include "simulation/SimConstants.h"

#include "robots/Robot.h"
#include "robots/WorkerRobot.h"
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

constexpr Robot::Config robotConfig = {
    {robotSpeed, robotRotationSpeed, robotSize},
    {robotProportionalGain, robotIntegralGain, robotMaxIntegralError},
};
} // namespace

RobotController::RobotController(const LogisticsMap& logisticsMap,
                                 const BlockingRobotManager& blockingRobotManager)
    : logisticsMap_(logisticsMap), routePlanner_(logisticsMap), routeFollower_(logisticsMap),
      chargingManager_(logisticsMap), emergencyStopController_(blockingRobotManager) {}

void RobotController::initialize() {
    robot_ = std::make_unique<WorkerRobot>(logisticsMap_.getRobotStartPosition(), robotConfig);
    taskFlow_.reset();
    emergencyStopController_.reset();
    routeFollower_.setActivePath(
        routePlanner_.buildPathToPickup(logisticsMap_.getRobotStartPosition()),
        logisticsMap_.getRobotStartPosition(), *robot_);
}

void RobotController::update(float deltaTime, const InputState& inputState) {
    if (robot_ == nullptr) {
        return;
    }

    emergencyStopController_.updateEmergencyStop(inputState, *robot_);
    if (emergencyStopController_.isEmergencyStopActive()) {
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

    if (emergencyStopController_.shouldPauseForObstacle(robotPosition,
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
        emergencyStopController_.isEmergencyStopActive(),
    };
}

void RobotController::startChargingTrip() {
    const Vector2 startPosition = robot_->getPosition();

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
    const Vector2 startPosition = robot_->getPosition();

    taskFlow_.startTripToPickup();
    robot_->setState(Robot::State::Arrived);
    routeFollower_.setActivePath(routePlanner_.buildPathToPickup(startPosition), startPosition,
                                 *robot_);
}

void RobotController::startDropoffTrip() {
    const Vector2 startPosition = robot_->getPosition();

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
                                                         robot_->getPosition())) {
        startChargingTrip();
        return;
    }

    startPickupTrip();
}

void RobotController::updateCharging(float deltaTime) {
    robot_->setState(Robot::State::Charging);
    robot_->getBattery().charge(SimConstants::kChargeRatePerSecond * deltaTime);

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
