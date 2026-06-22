#include "robots/Robot.h"

#include <cmath>

namespace {
constexpr double pi = 3.14159265358979323846;
constexpr float reachedDistance = 2.0f;
constexpr float radToDeg = 57.29577951308232f;
constexpr float fullTurn = 360.0f;
constexpr float halfTurn = 180.0f;
constexpr float batteryDrainPercentagePerPixel = 0.01f;

double DegreesToRadians(double degree) {
    return degree * pi / 180.0;
}

float Distance(Vector2 from, Vector2 to) {
    const float deltaX = to.x - from.x;
    const float deltaY = to.y - from.y;

    return std::sqrt((deltaX * deltaX) + (deltaY * deltaY));
}

float NormalizeAngle(float angle) {
    while (angle > halfTurn) {
        angle -= fullTurn;
    }

    while (angle < -halfTurn) {
        angle += fullTurn;
    }

    return angle;
}

float TargetRotation(Vector2 from, Vector2 to) {
    return std::atan2(to.y - from.y, to.x - from.x) * radToDeg;
}

Color GetRobotColor(Robot::State state) {
    switch (state) {
    case Robot::State::Idle:
        return GRAY;
    case Robot::State::Moving:
        return GREEN;
    case Robot::State::PickingUp:
        return YELLOW;
    case Robot::State::CarryingItem:
        return ORANGE;
    case Robot::State::DroppingOff:
        return SKYBLUE;
    case Robot::State::Arrived:
        return LIME;
    case Robot::State::BatteryDepleted:
        return RED;
    case Robot::State::Charging:
        return BLUE;
    }

    return ORANGE;
}

bool ShouldMove(Robot::State state) {
    return state == Robot::State::Moving || state == Robot::State::CarryingItem;
}

bool ShouldDrawItem(Robot::State state) {
    return state == Robot::State::PickingUp || state == Robot::State::CarryingItem ||
           state == Robot::State::DroppingOff;
}
} // namespace

Robot::Robot(Pose startPose, Config config)
    : x_(startPose.position.x), y_(startPose.position.y), angle_(startPose.angleDegrees),
      speed_(config.motion.speed), targetPosition_(startPose.position),
      rotationSpeed_(config.motion.rotationSpeed), size_(config.motion.size),
      speedController_(config.controller), state_(State::Idle) {}

Robot::Robot(const Vector2& startPosition, Config config) : Robot(Pose{startPosition}, config) {}

void Robot::updateMovement(float deltaTime) {
    if (battery_.isEmpty()) {
        state_ = State::BatteryDepleted;
        return;
    }

    if (!ShouldMove(state_)) {
        return;
    }

    const Vector2 previousPosition = {static_cast<float>(x_), static_cast<float>(y_)};

    rotateTowardsTarget(deltaTime);
    moveTowardsTarget(deltaTime);

    const Vector2 currentPosition = {static_cast<float>(x_), static_cast<float>(y_)};
    battery_.drain(Distance(previousPosition, currentPosition) * batteryDrainPercentagePerPixel);
    if (battery_.isEmpty()) {
        state_ = State::BatteryDepleted;
    }

    updateSensors();
}

void Robot::draw(void) {
    const Vector2 position = {static_cast<float>(x_), static_cast<float>(y_)};
    const float radius = size_;
    const float headingLength = radius * 1.15f;
    const float headingRadians = static_cast<float>(angle_) / radToDeg;
    const Vector2 headingEnd = {
        position.x + std::cos(headingRadians) * headingLength,
        position.y + std::sin(headingRadians) * headingLength,
    };

    DrawCircleV(position, radius, GetRobotColor(state_));
    DrawCircleLines((int)position.x, (int)position.y, radius, BLACK);
    DrawCircleLines((int)position.x, (int)position.y, radius + 2.0f, WHITE);
    DrawLineEx(position, headingEnd, 3.0f, BLACK);
    DrawCircleV(position, radius * 0.2f, BLACK);
    DrawText("R", (int)position.x - 5, (int)position.y - 10, 20, WHITE);

    if (ShouldDrawItem(state_)) {
        const Rectangle item = {position.x - 7.0f, position.y - 25.0f, 14.0f, 12.0f};

        DrawRectangleRec(item, GOLD);
        DrawRectangleLinesEx(item, 2.0f, BROWN);
    }
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

void Robot::moveTowardsTarget(float deltaTime) {
    const Vector2 position = {static_cast<float>(x_), static_cast<float>(y_)};
    const float distance = Distance(position, targetPosition_);
    if (distance <= reachedDistance) {
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

    const Vector2 position = {static_cast<float>(x_), static_cast<float>(y_)};
    const float desiredRotation = TargetRotation(position, targetPosition_);
    const float angleDifference = NormalizeAngle(desiredRotation - static_cast<float>(angle_));
    const float maxStep = rotationSpeed_ * deltaTime;

    if (std::fabs(angleDifference) <= maxStep) {
        angle_ = desiredRotation;
        return;
    }

    angle_ += angleDifference > 0.0f ? maxStep : -maxStep;
    angle_ = NormalizeAngle(static_cast<float>(angle_));
}

void Robot::getPosition(Vector2& outPosition) const {
    outPosition = {static_cast<float>(x_), static_cast<float>(y_)};
}

void Robot::getRotation(float& outRotation) const {
    outRotation = static_cast<float>(angle_);
}

Robot::State Robot::getState(void) const {
    return state_;
}

bool Robot::hasReachedTarget(void) const {
    const Vector2 position = {static_cast<float>(x_), static_cast<float>(y_)};

    return Distance(position, targetPosition_) <= reachedDistance;
}

Battery& Robot::getBattery(void) {
    return battery_;
}

const Battery& Robot::getBattery(void) const {
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

double Robot::x() const {
    return x_;
}

double Robot::y() const {
    return y_;
}

double Robot::angle() const {
    return angle_;
}

void Robot::moveForward(double distance) {
    const double radians = DegreesToRadians(angle_);

    x_ += std::cos(radians) * distance;
    y_ += std::sin(radians) * distance;
}

void Robot::rotate(double degree) {
    angle_ += degree;
}
