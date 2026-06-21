#include "simulation/RobotController.h"

#include "robots/Robot.h"
#include "robots/WorkerRobot.h"
#include "sensors/LidarSensor.h"
#include "simulation/BlockingRobotController.h"
#include "simulation/navigation.h"

#include <cstddef>
#include <cstdint>
#include <cmath>
#include <memory>
#include <vector>

namespace {
constexpr float robotSpeed = 120.0f;
constexpr float robotRotationSpeed = 180.0f;
constexpr float robotSize = 16.0f;
constexpr float pickupDuration = 1.0f;
constexpr float dropoffDuration = 1.0f;
constexpr float lidarDetectionRadius = robotSize * 2.0f;
constexpr float chargeRatePercentagePerSecond = 10.0f;
constexpr float chargeAfterDropoffThreshold = 10.0f;
constexpr float minimumBatteryAfterJob = 10.0f;
constexpr float batteryDrainPercentagePerPixel = 0.01f;

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
    ToCharging,
    Charging,
};

std::unique_ptr<Robot> robot;
LidarSensor lidarSensor(lidarDetectionRadius);
std::vector<Vector2> activePath;
Vector2 activePathStart = {0.0f, 0.0f};
std::size_t currentWaypointIndex = 0;
TaskPhase taskPhase = TaskPhase::ToPickup;
float stateTimer = 0.0f;

float Distance(Vector2 from, Vector2 to) {
    const float deltaX = to.x - from.x;
    const float deltaY = to.y - from.y;

    return std::sqrt((deltaX * deltaX) + (deltaY * deltaY));
}

Vector2 GetRobotPosition(void) {
    Vector2 robotPosition = {0.0f, 0.0f};
    robot->getPosition(robotPosition);
    return robotPosition;
}

std::vector<Vector2> BuildPathToPickup(Vector2 startPosition) {
    return FindNavigationPath(startPosition, GetLagerDockPosition(GetMapPickupLagerId()));
}

std::vector<Vector2> BuildPathToDropoff(Vector2 startPosition) {
    return FindNavigationPath(startPosition, GetLagerDockPosition(GetMapDeliveryLagerId()));
}

std::vector<Vector2> BuildPathToChargingStation(Vector2 startPosition) {
    return FindNavigationPath(startPosition, GetChargingStationDockPosition());
}

float PathDistance(Vector2 startPosition, const std::vector<Vector2>& path) {
    float distance = 0.0f;
    Vector2 previousPoint = startPosition;

    for (Vector2 waypoint : path) {
        distance += Distance(previousPoint, waypoint);
        previousPoint = waypoint;
    }

    return distance;
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

bool ShouldChargeAtOrBelow(float thresholdPercentage) {
    return robot->getBattery().getChargePercentage() <= thresholdPercentage;
}

void StartPickupTrip(void);
void StartDropoffTrip(void);

void StartChargingTrip(void) {
    const Vector2 startPosition = GetRobotPosition();

    taskPhase = TaskPhase::ToCharging;
    robot->setState(Robot::State::Arrived);
    SetActivePath(BuildPathToChargingStation(startPosition), startPosition);

    if (activePath.empty()) {
        taskPhase = TaskPhase::Charging;
        stateTimer = 0.0f;
        robot->setState(Robot::State::Charging);
    }
}

void StartPickupTrip(void) {
    const Vector2 startPosition = GetRobotPosition();

    taskPhase = TaskPhase::ToPickup;
    robot->setState(Robot::State::Arrived);
    SetActivePath(BuildPathToPickup(startPosition), startPosition);
}

void StartDropoffTrip(void) {
    const Vector2 startPosition = GetRobotPosition();

    taskPhase = TaskPhase::ToDropoff;
    robot->setState(Robot::State::CarryingItem);
    SetActivePath(BuildPathToDropoff(startPosition), startPosition);
}

bool CanCompleteNextDeliveryBeforeMinimumBattery(void) {
    const Vector2 robotPosition = GetRobotPosition();
    const Vector2 pickupDock = GetLagerDockPosition(GetMapPickupLagerId());
    const std::vector<Vector2> pickupPath = BuildPathToPickup(robotPosition);
    const std::vector<Vector2> dropoffPath = BuildPathToDropoff(pickupDock);
    const float estimatedDistance =
        PathDistance(robotPosition, pickupPath) + PathDistance(pickupDock, dropoffPath);

    const float estimatedBatteryAfterJob =
        robot->getBattery().getChargePercentage() -
        (estimatedDistance * batteryDrainPercentagePerPixel);

    return estimatedBatteryAfterJob > minimumBatteryAfterJob;
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

    StartDropoffTrip();
}

void UpdateDropoff(float deltaTime) {
    stateTimer += deltaTime;
    if (stateTimer < dropoffDuration) {
        return;
    }

    if (ShouldChargeAtOrBelow(chargeAfterDropoffThreshold) ||
        !CanCompleteNextDeliveryBeforeMinimumBattery()) {
        StartChargingTrip();
        return;
    }

    StartPickupTrip();
}

void UpdateCharging(float deltaTime) {
    robot->setState(Robot::State::Charging);
    robot->getBattery().charge(chargeRatePercentagePerSecond * deltaTime);

    if (!robot->getBattery().isFull()) {
        return;
    }

    StartPickupTrip();
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
        return;
    }

    if (taskPhase == TaskPhase::ToCharging) {
        taskPhase = TaskPhase::Charging;
        stateTimer = 0.0f;
        robot->setState(Robot::State::Charging);
    }
}
} // namespace

void InitRobotController(void) {
    robot = std::make_unique<WorkerRobot>(GetRobotStartPosition(), robotConfig);
    taskPhase = TaskPhase::ToPickup;
    stateTimer = 0.0f;
    SetActivePath(BuildPathToPickup(GetRobotStartPosition()), GetRobotStartPosition());
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

    if (taskPhase == TaskPhase::Charging) {
        UpdateCharging(deltaTime);
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

std::optional<RobotStatusSnapshot> GetRobotStatusSnapshot(void) {
    if (robot == nullptr) {
        return std::nullopt;
    }

    return RobotStatusSnapshot{
        robot->getState(),
        robot->getBattery().getChargePercentage(),
    };
}
