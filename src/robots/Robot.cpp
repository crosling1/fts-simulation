#include "robots/Robot.h"

#include "simulation/MathUtils.h"

#include <cmath>

namespace {
constexpr float fullTurn = 360.0f;
constexpr float halfTurn = 180.0f;

float DegreesToRadians(float degree) {
    return degree * math::kPi / halfTurn;
}

float NormalizeAngle(float angle) noexcept {
    angle = std::fmod(angle + halfTurn, fullTurn);
    if (angle < 0.0f) {
        angle += fullTurn;
    }
    return angle - halfTurn;
}

float TargetRotation(Vec2 from, Vec2 to) {
    return std::atan2(to.y - from.y, to.x - from.x) * math::kRadToDeg;
}

bool ShouldMove(Robot::State state) {
    return state == Robot::State::Moving || state == Robot::State::CarryingItem;
}

bool ShouldDrawItem(Robot::State state) {
    return state == Robot::State::PickingUp || state == Robot::State::CarryingItem ||
           state == Robot::State::DroppingOff;
}
} // namespace

Robot::Robot(Pose startPose, Config config, const SimConfig& simConfig)
    : x_(startPose.position.x), y_(startPose.position.y), angle_(startPose.angleDegrees),
      speed_(config.motion.speed), targetPosition_(startPose.position),
      rotationSpeed_(config.motion.rotationSpeed), size_(config.motion.size),
      speedController_(config.controller), simConfig_(simConfig), state_(State::Idle),
      proximitySensor_(config.motion.size * simConfig.sensorRangeMultiplier) {}

Robot::Robot(const Vec2& startPosition, Config config, const SimConfig& simConfig)
    : Robot(Pose{startPosition}, config, simConfig) {}

void Robot::updateMovement(float deltaTime) {
    if (battery_.isEmpty()) {
        state_ = State::BatteryDepleted;
        return;
    }

    if (!ShouldMove(state_)) {
        return;
    }

    const Vec2 previousPosition = {x_, y_};

    rotateTowardsTarget(deltaTime);
    moveTowardsTarget(deltaTime);

    const Vec2 currentPosition = {x_, y_};
    battery_.drain(math::distance(previousPosition, currentPosition) *
                   simConfig_.batteryDrainPerPixel);
    if (battery_.isEmpty()) {
        state_ = State::BatteryDepleted;
    }

    updateSensors();
}

RobotRenderData Robot::renderData() const noexcept {
    return {
        {x_, y_},   angle_,
        size_,      proximitySensor_.getDetectionRadius(),
        state_,     ShouldDrawItem(state_),
        typeName(),
    };
}

void Robot::setPosition(const Vec2& newPosition) {
    x_ = newPosition.x;
    y_ = newPosition.y;
    speedController_.reset();
}

void Robot::setState(State newState) {
    state_ = newState;
}

void Robot::setTargetPosition(const Vec2& target) {
    beginMovingTo(target);
}

void Robot::enterIdle() {
    state_ = State::Idle;
}

void Robot::arriveAtWaypoint() {
    state_ = State::Arrived;
}

void Robot::beginMovingTo(Vec2 target) {
    targetPosition_ = target;
    speedController_.reset();
    if (hasReachedTarget()) {
        arriveAtWaypoint();
        return;
    }

    if (battery_.isEmpty()) {
        state_ = State::BatteryDepleted;
        return;
    }

    if (state_ != State::CarryingItem) {
        state_ = State::Moving;
    }
}

void Robot::beginCarrying() {
    state_ = State::CarryingItem;
}

void Robot::beginPickingUp() {
    state_ = State::PickingUp;
}

void Robot::beginDroppingOff() {
    state_ = State::DroppingOff;
}

void Robot::chargeBy(float amount) {
    battery_.charge(amount);
}

void Robot::enterChargingState() {
    state_ = State::Charging;
}

void Robot::moveTowardsTarget(float deltaTime) {
    const Vec2 position = {x_, y_};
    const float distance = math::distance(position, targetPosition_);
    if (distance <= simConfig_.reachedDistance) {
        snapToTarget();
        return;
    }

    const float controlledSpeed = speedController_.update({distance, deltaTime, speed_});
    const float step = controlledSpeed * deltaTime;
    if (step >= distance) {
        snapToTarget();
        return;
    }

    const float directionX = (targetPosition_.x - position.x) / distance;
    const float directionY = (targetPosition_.y - position.y) / distance;

    x_ += directionX * step;
    y_ += directionY * step;
}

void Robot::snapToTarget() {
    x_ = targetPosition_.x;
    y_ = targetPosition_.y;
    speedController_.reset();
    if (state_ != State::CarryingItem) {
        arriveAtWaypoint();
    }
}

void Robot::rotateTowardsTarget(float deltaTime) {
    if (hasReachedTarget()) {
        return;
    }

    const Vec2 position = {x_, y_};
    const float desiredRotation = TargetRotation(position, targetPosition_);
    const float angleDifference = NormalizeAngle(desiredRotation - angle_);
    const float maxStep = rotationSpeed_ * deltaTime;

    if (std::fabs(angleDifference) <= maxStep) {
        angle_ = desiredRotation;
        return;
    }

    angle_ += angleDifference > 0.0f ? maxStep : -maxStep;
    angle_ = NormalizeAngle(angle_);
}

Vec2 Robot::getPosition() const {
    return {x_, y_};
}

Robot::State Robot::getState() const {
    return state_;
}

bool Robot::hasBatteryFull() const {
    return battery_.isFull();
}

bool Robot::hasReachedTarget() const {
    const Vec2 position = {x_, y_};

    return math::distance(position, targetPosition_) <= simConfig_.reachedDistance;
}

float Robot::getProximityDetectionRadius() const {
    return proximitySensor_.getDetectionRadius();
}

const Battery& Robot::getBattery() const {
    return battery_;
}

void Robot::addSensor(std::unique_ptr<Sensor> sensor) {
    sensors_.push_back(std::move(sensor));
}

void Robot::updateSensors() {
    for (const std::unique_ptr<Sensor>& sensor : sensors_) {
        sensor->update(*this);
    }
}

float Robot::x() const noexcept {
    return x_;
}

float Robot::y() const noexcept {
    return y_;
}

float Robot::angle() const noexcept {
    return angle_;
}

void Robot::moveForward(float distance) {
    const float radians = DegreesToRadians(angle_);

    x_ += std::cos(radians) * distance;
    y_ += std::sin(radians) * distance;
}

void Robot::rotate(float degree) {
    angle_ += degree;
}
