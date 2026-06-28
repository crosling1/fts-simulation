#include "simulation/RobotController.h"

#include "robots/Robot.h"
#include "robots/WorkerRobot.h"
#include "simulation/map.h"

#include <memory>
#include <vector>

RobotController::RobotController(const LogisticsMap& logisticsMap,
                                 const BlockingRobotManager& blockingRobotManager,
                                 const SimConfig& simConfig)
    : logisticsMap_(logisticsMap), simConfig_(simConfig), routePlanner_(logisticsMap),
      routeFollower_(logisticsMap), chargingManager_(logisticsMap, simConfig),
      emergencyStopController_(blockingRobotManager), taskFlow_(simConfig) {}

void RobotController::initialize() {
    const Robot::Config robotConfig = {
        {simConfig_.robotSpeed, simConfig_.robotRotationSpeed, simConfig_.robotSize},
        {simConfig_.robotProportionalGain, simConfig_.robotIntegralGain,
         simConfig_.robotMaxIntegralError},
    };

    robot_ = std::make_unique<WorkerRobot>(logisticsMap_.getRobotStartPosition(), robotConfig,
                                           simConfig_);
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

    if (updateStationaryTask(deltaTime)) {
        return;
    }

    const Vector2 robotPosition = robot_->getPosition();

    // Obstacle proximity is checked only during movement phases.
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

    robotRenderer_.draw(robot_->renderData());
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
        startCharging();
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

void RobotController::startCharging() {
    taskFlow_.startCharging();
    robot_->enterChargingState();
}

bool RobotController::updateStationaryTask(float deltaTime) {
    if (taskFlow_.isPickingUp()) {
        updatePickup(deltaTime);
        return true;
    }

    if (taskFlow_.isDroppingOff()) {
        updateDropoff(deltaTime);
        return true;
    }

    if (taskFlow_.isCharging()) {
        updateCharging(deltaTime);
        return true;
    }

    return false;
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
    robot_->chargeBy(simConfig_.batteryChargeRatePerSecond * deltaTime);

    if (!robot_->hasBatteryFull()) {
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
        startCharging();
    }
}
