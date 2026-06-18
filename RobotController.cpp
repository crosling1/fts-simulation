#include "RobotController.h"

#include "LidarSensor.h"
#include "navigation.h"
#include "ObstacleManager.h"
#include "Robot.h"

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
constexpr float blockingRobotRadius = 14.0f;
constexpr float blockingRobotSpeed = 65.0f;

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
ObstacleManager obstacleManager;
LidarSensor lidarSensor(lidarDetectionRadius);
std::vector<Vector2> activePath;
Vector2 activePathStart = {0.0f, 0.0f};
std::size_t currentWaypointIndex = 0;
TaskPhase taskPhase = TaskPhase::ToPickup;
float stateTimer = 0.0f;

std::vector<Vector2> BuildPathToPointA(void) {
    std::vector<Vector2> path =
        FindNavigationPath(GetRobotStartPosition(), GetLagerDockPosition(LAGER_2));
    if (!path.empty()) {
        return path;
    }

    return {
        {200.0f, 620.0f}, {200.0f, 450.0f}, {800.0f, 450.0f},
        {800.0f, 270.0f}, {450.0f, 270.0f}, GetLagerDockPosition(LAGER_2),
    };
}

std::vector<Vector2> BuildPathFromPointAToPointB(void) {
    std::vector<Vector2> path =
        FindNavigationPath(GetLagerDockPosition(LAGER_2), GetLagerDockPosition(LAGER_5));
    if (!path.empty()) {
        return path;
    }

    return {
        {450.0f, 270.0f}, {800.0f, 270.0f}, {800.0f, 450.0f},
        {960.0f, 450.0f}, {960.0f, 556.0f}, GetLagerDockPosition(LAGER_5),
    };
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

void AddBlockingRobot(const std::vector<Vector2>& path, float speed) {
    if (path.size() < 2) {
        return;
    }

    obstacleManager.addObstacle({
        path[0],
        blockingRobotRadius,
        speed,
        path,
        0,
        1,
        0,
        true,
    });
}

void InitBlockingRobots(void) {
    obstacleManager.clear();
    const std::vector<Vector2> mainRoadNetwork = {
        {140.0f, 620.0f}, {200.0f, 620.0f},  {200.0f, 450.0f}, {540.0f, 450.0f}, {800.0f, 450.0f},
        {960.0f, 450.0f}, {1040.0f, 450.0f}, {960.0f, 450.0f}, {960.0f, 350.0f}, {800.0f, 350.0f},
        {800.0f, 270.0f}, {540.0f, 270.0f},  {450.0f, 270.0f}, {340.0f, 270.0f},
    };
    const std::vector<Vector2> upperRoadNetwork = {
        {540.0f, 150.0f}, {540.0f, 270.0f}, {675.0f, 270.0f}, {800.0f, 270.0f}, {800.0f, 150.0f},
        {800.0f, 270.0f}, {800.0f, 450.0f}, {960.0f, 450.0f}, {960.0f, 556.0f}, {930.0f, 556.0f},
    };
    const std::vector<Vector2> warehouseRoadNetwork = {
        {340.0f, 325.0f}, {340.0f, 270.0f}, {450.0f, 270.0f}, {450.0f, 224.0f}, {450.0f, 270.0f},
        {675.0f, 270.0f}, {675.0f, 224.0f}, {675.0f, 270.0f}, {800.0f, 270.0f}, {850.0f, 257.0f},
    };

    AddBlockingRobot(mainRoadNetwork, blockingRobotSpeed);
    AddBlockingRobot(upperRoadNetwork, blockingRobotSpeed * 0.85f);
    AddBlockingRobot(warehouseRoadNetwork, blockingRobotSpeed * 1.15f);
    AddBlockingRobot(
        {
            {800.0f, 150.0f},
            {800.0f, 270.0f},
            {800.0f, 350.0f},
            {800.0f, 470.0f},
        },
        blockingRobotSpeed * 1.2f);
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
    SetActivePath(BuildPathFromPointAToPointB(), GetLagerDockPosition(LAGER_2));
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
    robot = std::make_unique<Robot>(GetRobotStartPosition(), robotConfig);
    taskPhase = TaskPhase::ToPickup;
    stateTimer = 0.0f;
    InitBlockingRobots();
    SetActivePath(BuildPathToPointA(), GetRobotStartPosition());
}

void UpdateRobotController(void) {
    if (robot == nullptr) {
        return;
    }

    const float deltaTime = GetFrameTime();
    Vector2 robotPosition = {0.0f, 0.0f};
    robot->getPosition(robotPosition);
    obstacleManager.update(deltaTime);

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

    if (lidarSensor.hasObstacleNearby(robotPosition, obstacleManager)) {
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

    obstacleManager.draw();
    lidarSensor.drawScanArea(robotPosition);
    robot->draw();
}

void UnloadRobotController(void) {
    robot.reset();
}
