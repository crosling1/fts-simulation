#include "simulation/RobotController.h"

#include "robots/Robot.h"
#include "robots/WorkerRobot.h"
#include "simulation/map.h"

#include <memory>
#include <utility>
#include <vector>

RobotController::RobotController(const LogisticsMap& logisticsMap,
                                 const BlockingRobotManager& blockingRobotManager,
                                 const SimConfig& simConfig)
    : logisticsMap_(logisticsMap), simConfig_(simConfig), routePlanner_(logisticsMap),
      routeFollower_(logisticsMap), chargingManager_(simConfig),
      emergencyStopController_(blockingRobotManager), taskFlow_(simConfig) {}

void RobotController::initialize() {
    routeFollower_.reset();

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

    const Vec2 robotPosition = robot_->getPosition();

    // Obstacle proximity is checked only during movement phases.
    if (emergencyStopController_.shouldPauseForObstacle(robotPosition,
                                                        robot_->getProximityDetectionRadius())) {
        return;
    }

    robot_->updateMovement(deltaTime);
    routeFollower_.keepOnRoad(*robot_);
    updateWaypointTravel();
}

void RobotController::unload() {
    robot_.reset();
}

const RouteFollower& RobotController::routeFollower() const noexcept {
    return routeFollower_;
}

std::optional<RobotRenderData> RobotController::robotRenderData() const {
    if (robot_ == nullptr) {
        return std::nullopt;
    }

    return robot_->renderData();
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
    const Vec2 startPosition = robot_->getPosition();

    taskFlow_.startTripToCharging();
    robot_->arriveAtWaypoint();
    const std::vector<Vec2> chargingPath = routePlanner_.buildPathToChargingStation(startPosition);
    routeFollower_.setActivePath(chargingPath, startPosition, *robot_);

    if (chargingPath.empty()) {
        startCharging();
    }
}

void RobotController::startPickupTrip() {
    const Vec2 startPosition = robot_->getPosition();
    startPickupTrip(routePlanner_.buildPathToPickup(startPosition));
}

void RobotController::startPickupTrip(const std::vector<Vec2>& pickupPath) {
    const Vec2 startPosition = robot_->getPosition();

    taskFlow_.startTripToPickup();
    robot_->arriveAtWaypoint();
    routeFollower_.setActivePath(pickupPath, startPosition, *robot_);
}

void RobotController::startDropoffTrip() {
    const Vec2 startPosition = robot_->getPosition();

    taskFlow_.startTripToDropoff();
    robot_->beginCarrying();
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

    const NextDeliveryRouteEstimate nextDeliveryEstimate = buildNextDeliveryRouteEstimate();

    if (chargingManager_.shouldStartChargingAfterDropoff(*robot_, nextDeliveryEstimate.distance)) {
        startChargingTrip();
        return;
    }

    startPickupTrip(nextDeliveryEstimate.pickupPath);
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
        robot_->beginPickingUp();
        return;
    }

    if (taskFlow_.isRoutingToDropoff()) {
        taskFlow_.startDroppingOff();
        robot_->beginDroppingOff();
        return;
    }

    if (taskFlow_.isRoutingToCharging()) {
        startCharging();
    }
}

RobotController::NextDeliveryRouteEstimate RobotController::buildNextDeliveryRouteEstimate() const {
    const auto pickupDock = logisticsMap_.getPickupDockPosition();
    if (!pickupDock) {
        return {};
    }

    const Vec2 robotPosition = robot_->getPosition();
    std::vector<Vec2> pickupPath = routePlanner_.buildPathToPickup(robotPosition);
    const std::vector<Vec2> dropoffPath = routePlanner_.buildPathToDropoff(*pickupDock);
    const float estimatedDistance = routePlanner_.calculatePathDistance(robotPosition, pickupPath) +
                                    routePlanner_.calculatePathDistance(*pickupDock, dropoffPath);

    return {std::move(pickupPath), estimatedDistance};
}
