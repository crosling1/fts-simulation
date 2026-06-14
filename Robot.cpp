#include "Robot.h"

#include <cmath>

namespace {
constexpr float reachedDistance = 2.0f;
constexpr float radToDeg = 57.29577951308232f;
constexpr float fullTurn = 360.0f;
constexpr float halfTurn = 180.0f;

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

Robot::Robot(const Vector2& startPosition, Config config)
    : speed(config.speed), position(startPosition), targetPosition(startPosition), rotation(0.0f),
      rotationSpeed(config.rotationSpeed), size(config.size), state(State::Idle) {}

void Robot::update(float deltaTime) {
    if (!ShouldMove(state)) {
        return;
    }

    rotateTowardsTarget(deltaTime);
    moveTowardsTarget(deltaTime);
}

void Robot::draw(void) {
    const float radius = size;
    const float headingLength = radius * 1.15f;
    const float headingRadians = rotation / radToDeg;
    const Vector2 headingEnd = {
        position.x + std::cos(headingRadians) * headingLength,
        position.y + std::sin(headingRadians) * headingLength,
    };

    DrawCircleV(position, radius, GetRobotColor(state));
    DrawCircleLines((int)position.x, (int)position.y, radius, BLACK);
    DrawCircleLines((int)position.x, (int)position.y, radius + 2.0f, WHITE);
    DrawLineEx(position, headingEnd, 3.0f, BLACK);
    DrawCircleV(position, radius * 0.2f, BLACK);
    DrawText("R", (int)position.x - 5, (int)position.y - 10, 20, WHITE);

    if (ShouldDrawItem(state)) {
        const Rectangle item = {position.x - 7.0f, position.y - 25.0f, 14.0f, 12.0f};

        DrawRectangleRec(item, GOLD);
        DrawRectangleLinesEx(item, 2.0f, BROWN);
    }
}

void Robot::setPosition(const Vector2& newPosition) {
    position = newPosition;
}

void Robot::setState(State newState) {
    state = newState;
}

void Robot::setTargetPosition(const Vector2& target) {
    targetPosition = target;
    if (hasReachedTarget()) {
        state = State::Arrived;
        return;
    }

    if (state != State::CarryingItem) {
        state = State::Moving;
    }
}

void Robot::moveTowardsTarget(float deltaTime) {
    const float distance = Distance(position, targetPosition);
    if (distance <= reachedDistance) {
        position = targetPosition;
        if (state != State::CarryingItem) {
            state = State::Arrived;
        }
        return;
    }

    const float step = speed * deltaTime;
    if (step >= distance) {
        position = targetPosition;
        if (state != State::CarryingItem) {
            state = State::Arrived;
        }
        return;
    }

    const float directionX = (targetPosition.x - position.x) / distance;
    const float directionY = (targetPosition.y - position.y) / distance;

    position.x += directionX * step;
    position.y += directionY * step;
}

void Robot::rotateTowardsTarget(float deltaTime) {
    if (hasReachedTarget()) {
        return;
    }

    const float desiredRotation = TargetRotation(position, targetPosition);
    const float angleDifference = NormalizeAngle(desiredRotation - rotation);
    const float maxStep = rotationSpeed * deltaTime;

    if (std::fabs(angleDifference) <= maxStep) {
        rotation = desiredRotation;
        return;
    }

    rotation += angleDifference > 0.0f ? maxStep : -maxStep;
    rotation = NormalizeAngle(rotation);
}

void Robot::getPosition(Vector2& outPosition) const {
    outPosition = position;
}

void Robot::getRotation(float& outRotation) const {
    outRotation = rotation;
}

Robot::State Robot::getState(void) const {
    return state;
}

bool Robot::hasReachedTarget(void) const {
    return Distance(position, targetPosition) <= reachedDistance;
}
