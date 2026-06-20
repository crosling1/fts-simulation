#include "simulation/RobotController.h"

#include "robots/Robot.h"
#include "robots/WorkerRobot.h"
#include "sensors/LidarSensor.h"
#include "simulation/BlockingRobotController.h"
#include "simulation/navigation.h"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

namespace {
constexpr float robotSpeed = 120.0f;
constexpr float robotRotationSpeed = 180.0f;
constexpr float robotSize = 16.0f;
constexpr float pickupDuration = 1.0f;
constexpr float dropoffDuration = 1.0f;
constexpr float lidarDetectionRadius = robotSize * 2.0f;

constexpr Robot::Config robotConfig = {
    robotSpeed,
    robotRotationSpeed,
    robotSize,
};

enum class TaskPhase : std::uint8_t {
    ToPickup,
    PickingUp,
    ToDropoff,
    DroppingOff,
    Complete,
};

std::unique_ptr<Robot> robot;
LidarSensor lidarSensor(lidarDetectionRadius);
std::vector<Vector2> activePath;
Vector2 activePathStart = {0.0f, 0.0f};
std::size_t currentWaypointIndex = 0;
TaskPhase taskPhase = TaskPhase::ToPickup;
float stateTimer = 0.0f;

std::vector<Vector2> BuildPathToPointA(void) {
    return FindNavigationPath(GetRobotStartPosition(), GetLagerDockPosition(GetMapPickupLagerId()));
}

std::vector<Vector2> BuildPathFromPointAToPointB(void) {
    return FindNavigationPath(GetLagerDockPosition(GetMapPickupLagerId()),
                              GetLagerDockPosition(GetMapDeliveryLagerId()));
}

bool SetNextWaypoint(void) {
    if (robot == nullptr || currentWaypointIndex >= activePath.size()) {
        return false;
    }

    robot->setTargetPosition(activePath[currentWaypointIndex]);
    currentWaypointIndex++;
    return true;
}

void SetActivePath(const std::vector<Vector2>& path, Vector2 pathStart) {
    activePath = path;
    activePathStart = pathStart;
    currentWaypointIndex = 0;
    SetNextWaypoint();
}

void KeepRobotOnRoad(void) {
    Vector2 robotPosition = {0.0f, 0.0f};
    robot->getPosition(robotPosition);
    if (!IsMapRoadPosition(robotPosition)) {
        robot->setPosition(ClampPositionToMapRoad(robotPosition));
    }
}

void UpdatePickup(float deltaTime) {
    stateTimer += deltaTime;
    if (stateTimer < pickupDuration) {
        return;
    }

    taskPhase = TaskPhase::ToDropoff;
    robot->setState(Robot::State::CarryingItem);
    SetActivePath(BuildPathFromPointAToPointB(), GetLagerDockPosition(GetMapPickupLagerId()));
}

void UpdateDropoff(float deltaTime) {
    stateTimer += deltaTime;
    if (stateTimer < dropoffDuration) {
        return;
    }

    taskPhase = TaskPhase::Complete;
    robot->setState(Robot::State::Arrived);
}

void UpdateWaypointTravel(void) {
    if (!robot->hasReachedTarget()) {
        return;
    }

    if (SetNextWaypoint()) {
        return;
    }

    if (taskPhase == TaskPhase::ToPickup) {
        taskPhase = TaskPhase::PickingUp;
        stateTimer = 0.0f;
        robot->setState(Robot::State::PickingUp);
        return;
    }

    if (taskPhase == TaskPhase::ToDropoff) {
        taskPhase = TaskPhase::DroppingOff;
        stateTimer = 0.0f;
        robot->setState(Robot::State::DroppingOff);
    }
}
} // namespace

void InitRobotController(void) {
    robot = std::make_unique<WorkerRobot>(GetRobotStartPosition(), robotConfig);
    taskPhase = TaskPhase::ToPickup;
    stateTimer = 0.0f;
    SetActivePath(BuildPathToPointA(), GetRobotStartPosition());
}

void UpdateRobotController(void) {
    if (robot == nullptr) {
        return;
    }

    const float deltaTime = GetFrameTime();
    Vector2 robotPosition = {0.0f, 0.0f};
    robot->getPosition(robotPosition);

    if (taskPhase == TaskPhase::PickingUp) {
        UpdatePickup(deltaTime);
        return;
    }

    if (taskPhase == TaskPhase::DroppingOff) {
        UpdateDropoff(deltaTime);
        return;
    }

    if (taskPhase == TaskPhase::Complete) {
        return;
    }

    if (HasBlockingRobotNear(robotPosition, lidarSensor.getDetectionRadius())) {
        return;
    }

    robot->update(deltaTime);
    KeepRobotOnRoad();
    UpdateWaypointTravel();
}

void DrawRobotController(void) {
    if (robot == nullptr) {
        return;
    }

    Vector2 previousPoint = activePathStart;
    for (Vector2 waypoint : activePath) {
        DrawLineEx(previousPoint, waypoint, 3.0f, MAGENTA);
        DrawCircleV(waypoint, 5.0f, MAGENTA);
        previousPoint = waypoint;
    }

    Vector2 robotPosition = {0.0f, 0.0f};
    robot->getPosition(robotPosition);

    lidarSensor.drawScanArea(robotPosition);
    robot->draw();
}

void UnloadRobotController(void) {
    robot.reset();
}
