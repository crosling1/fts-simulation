#include "robots/Robot.h"

#include <cmath>
#include <string_view>

namespace {
constexpr float pi = 3.14159265358979323846f;
constexpr float radToDeg = 57.29577951308232f;
constexpr float fullTurn = 360.0f;
constexpr float halfTurn = 180.0f;

float DegreesToRadians(float degree) {
    return degree * pi / halfTurn;
}

float Distance(Vector2 from, Vector2 to) {
    const float deltaX = to.x - from.x;
    const float deltaY = to.y - from.y;

    return std::sqrt((deltaX * deltaX) + (deltaY * deltaY));
}

float NormalizeAngle(float angle) noexcept {
    angle = std::fmod(angle + halfTurn, fullTurn);
    if (angle < 0.0f) {
        angle += fullTurn;
    }
    return angle - halfTurn;
}

float TargetRotation(Vector2 from, Vector2 to) {
    return std::atan2(to.y - from.y, to.x - from.x) * radToDeg;
}

bool ShouldMove(Robot::State state) {
    return state == Robot::State::Moving || state == Robot::State::CarryingItem;
}

bool ShouldDrawItem(Robot::State state) {
    return state == Robot::State::PickingUp || state == Robot::State::CarryingItem ||
           state == Robot::State::DroppingOff;
}
} // namespace

Robot::Robot(Pose startPose, Config config, SimConfig simConfig)
    : x_(startPose.position.x), y_(startPose.position.y), angle_(startPose.angleDegrees),
      speed_(config.motion.speed), targetPosition_(startPose.position),
      rotationSpeed_(config.motion.rotationSpeed), size_(config.motion.size),
      speedController_(config.controller), simConfig_(simConfig), state_(State::Idle),
      proximitySensor_(config.motion.size * simConfig.sensorRangeMultiplier) {}

Robot::Robot(const Vector2& startPosition, Config config, SimConfig simConfig)
    : Robot(Pose{startPosition}, config, simConfig) {}

void Robot::updateMovement(float deltaTime) {
    if (battery_.isEmpty()) {
        state_ = State::BatteryDepleted;
        return;
    }

    if (!ShouldMove(state_)) {
        return;
    }

    const Vector2 previousPosition = {x_, y_};

    rotateTowardsTarget(deltaTime);
    moveTowardsTarget(deltaTime);

    const Vector2 currentPosition = {x_, y_};
    battery_.drain(Distance(previousPosition, currentPosition) * simConfig_.batteryDrainPerPixel);
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

void Robot::setPosition(const Vector2& newPosition) {
    x_ = newPosition.x;
    y_ = newPosition.y;
    speedController_.reset();
}

void Robot::setState(State newState) {
    state_ = newState;
}

void Robot::setTargetPosition(const Vector2& target) {
    targetPosition_ = target;
    speedController_.reset();
    if (hasReachedTarget()) {
        state_ = State::Arrived;
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

void Robot::chargeBy(float amount) {
    battery_.charge(amount);
}

void Robot::enterChargingState() {
    state_ = State::Charging;
}

void Robot::moveTowardsTarget(float deltaTime) {
    const Vector2 position = {x_, y_};
    const float distance = Distance(position, targetPosition_);
    if (distance <= simConfig_.reachedDistance) {
        x_ = targetPosition_.x;
        y_ = targetPosition_.y;
        speedController_.reset();
        if (state_ != State::CarryingItem) {
            state_ = State::Arrived;
        }
        return;
    }

    const float controlledSpeed = speedController_.update({distance, deltaTime, speed_});
    const float step = controlledSpeed * deltaTime;
    if (step >= distance) {
        x_ = targetPosition_.x;
        y_ = targetPosition_.y;
        speedController_.reset();
        if (state_ != State::CarryingItem) {
            state_ = State::Arrived;
        }
        return;
    }

    const float directionX = (targetPosition_.x - position.x) / distance;
    const float directionY = (targetPosition_.y - position.y) / distance;

    x_ += directionX * step;
    y_ += directionY * step;
}

void Robot::rotateTowardsTarget(float deltaTime) {
    if (hasReachedTarget()) {
        return;
    }

    const Vector2 position = {x_, y_};
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

Vector2 Robot::getPosition() const {
    return {x_, y_};
}

Robot::State Robot::getState() const {
    return state_;
}

bool Robot::hasBatteryFull() const {
    return battery_.isFull();
}

bool Robot::hasReachedTarget() const {
    const Vector2 position = {x_, y_};

    return Distance(position, targetPosition_) <= simConfig_.reachedDistance;
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
