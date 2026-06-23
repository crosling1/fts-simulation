#include "simulation/RobotController.h"

#include "robots/Robot.h"
#include "robots/WorkerRobot.h"
#include "simulation/BlockingRobotManager.h"
#include "simulation/map.h"

#include <cstddef>
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
constexpr float chargeAfterDropoffThreshold = 10.0f;
constexpr float minimumBatteryAfterJob = 10.0f;
constexpr float batteryDrainPercentagePerPixel = 0.01f;

constexpr Robot::Config robotConfig = {
    {robotSpeed, robotRotationSpeed, robotSize},
    {robotProportionalGain, robotIntegralGain, robotMaxIntegralError},
};
} // namespace

RobotController::RobotController(const LogisticsMap& logisticsMap,
                                 const BlockingRobotManager& blockingRobotManager)
    : logisticsMap_(logisticsMap), blockingRobotManager_(blockingRobotManager),
      routePlanner_(logisticsMap) {}

void RobotController::initialize() {
    robot_ = std::make_unique<WorkerRobot>(logisticsMap_.getRobotStartPosition(), robotConfig);
    taskFlow_.reset();
    emergencyStopActive_ = false;
    stateBeforeEmergencyStop_ = Robot::State::Idle;
    setActivePath(routePlanner_.buildPathToPickup(logisticsMap_.getRobotStartPosition()),
                  logisticsMap_.getRobotStartPosition());
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
    keepRobotOnRoad();
    updateWaypointTravel();
}

void RobotController::draw() const {
    if (robot_ == nullptr) {
        return;
    }

    Vector2 previousPoint = activePathStart_;
    for (Vector2 waypoint : activePath_) {
        DrawLineEx(previousPoint, waypoint, 3.0f, MAGENTA);
        DrawCircleV(waypoint, 5.0f, MAGENTA);
        previousPoint = waypoint;
    }

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

bool RobotController::setNextWaypoint() {
    if (robot_ == nullptr || currentWaypointIndex_ >= activePath_.size()) {
        return false;
    }

    robot_->setTargetPosition(activePath_[currentWaypointIndex_]);
    currentWaypointIndex_++;
    return true;
}

void RobotController::setActivePath(const std::vector<Vector2>& path, Vector2 pathStart) {
    activePath_ = path;
    activePathStart_ = pathStart;
    currentWaypointIndex_ = 0;
    setNextWaypoint();
}

bool RobotController::shouldChargeAtOrBelow(float thresholdPercentage) const {
    return robot_->getBattery().getChargePercentage() <= thresholdPercentage;
}

void RobotController::startChargingTrip() {
    const Vector2 startPosition = getRobotPosition();

    taskFlow_.startTripToCharging();
    robot_->setState(Robot::State::Arrived);
    setActivePath(routePlanner_.buildPathToChargingStation(startPosition), startPosition);

    if (activePath_.empty()) {
        taskFlow_.startCharging();
        robot_->setState(Robot::State::Charging);
    }
}

void RobotController::startPickupTrip() {
    const Vector2 startPosition = getRobotPosition();

    taskFlow_.startTripToPickup();
    robot_->setState(Robot::State::Arrived);
    setActivePath(routePlanner_.buildPathToPickup(startPosition), startPosition);
}

void RobotController::startDropoffTrip() {
    const Vector2 startPosition = getRobotPosition();

    taskFlow_.startTripToDropoff();
    robot_->setState(Robot::State::CarryingItem);
    setActivePath(routePlanner_.buildPathToDropoff(startPosition), startPosition);
}

bool RobotController::canCompleteNextDeliveryBeforeMinimumBattery() const {
    const Vector2 robotPosition = getRobotPosition();
    const Vector2 pickupDock = logisticsMap_.getLagerDockPosition(logisticsMap_.getPickupLagerId());
    const std::vector<Vector2> pickupPath = routePlanner_.buildPathToPickup(robotPosition);
    const std::vector<Vector2> dropoffPath = routePlanner_.buildPathToDropoff(pickupDock);
    const float estimatedDistance = routePlanner_.calculatePathDistance(robotPosition, pickupPath) +
                                    routePlanner_.calculatePathDistance(pickupDock, dropoffPath);

    const float estimatedBatteryAfterJob = robot_->getBattery().getChargePercentage() -
                                           (estimatedDistance * batteryDrainPercentagePerPixel);

    return estimatedBatteryAfterJob > minimumBatteryAfterJob;
}

void RobotController::keepRobotOnRoad() {
    const Vector2 robotPosition = robot_->getPosition();
    if (!logisticsMap_.isRoadPosition(robotPosition)) {
        robot_->setPosition(logisticsMap_.clampPositionToRoad(robotPosition));
    }
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

    if (shouldChargeAtOrBelow(chargeAfterDropoffThreshold) ||
        !canCompleteNextDeliveryBeforeMinimumBattery()) {
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
    if (!robot_->hasReachedTarget()) {
        return;
    }

    if (setNextWaypoint()) {
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
