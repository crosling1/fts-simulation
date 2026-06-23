#ifndef ROBOT_CONTROLLER_H
#define ROBOT_CONTROLLER_H

#include "robots/Robot.h"
#include "simulation/InputState.h"
#include "simulation/RobotStatusSnapshot.h"

#include "raylib.h"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <vector>

class LogisticsMap;

class RobotController {
  public:
    explicit RobotController(const LogisticsMap& logisticsMap);

    void initialize();
    void update(float deltaTime, const InputState& inputState);
    void draw() const;
    void unload();
    std::optional<RobotStatusSnapshot> statusSnapshot() const;

  private:
    enum class TaskPhase : std::uint8_t {
        ToPickup,
        PickingUp,
        ToDropoff,
        DroppingOff,
        ToCharging,
        Charging,
    };

    Vector2 getRobotPosition() const;
    std::vector<Vector2> buildPathToPickup(Vector2 startPosition) const;
    std::vector<Vector2> buildPathToDropoff(Vector2 startPosition) const;
    std::vector<Vector2> buildPathToChargingStation(Vector2 startPosition) const;
    float pathDistance(Vector2 startPosition, const std::vector<Vector2>& path) const;
    bool setNextWaypoint();
    void setActivePath(const std::vector<Vector2>& path, Vector2 pathStart);
    bool shouldChargeAtOrBelow(float thresholdPercentage) const;
    void startPickupTrip();
    void startDropoffTrip();
    void startChargingTrip();
    bool canCompleteNextDeliveryBeforeMinimumBattery() const;
    void keepRobotOnRoad();
    void updatePickup(float deltaTime);
    void updateDropoff(float deltaTime);
    void updateCharging(float deltaTime);
    void updateWaypointTravel();
    void updateEmergencyStop(const InputState& inputState);

    const LogisticsMap& logisticsMap_;
    std::unique_ptr<Robot> robot_;
    std::vector<Vector2> activePath_;
    Vector2 activePathStart_ = {0.0f, 0.0f};
    std::size_t currentWaypointIndex_ = 0;
    TaskPhase taskPhase_ = TaskPhase::ToPickup;
    float stateTimer_ = 0.0f;
    bool emergencyStopActive_ = false;
    Robot::State stateBeforeEmergencyStop_ = Robot::State::Idle;
};

void InitRobotController(const LogisticsMap& logisticsMap);
void UpdateRobotController(float deltaTime, const InputState& inputState);
void DrawRobotController(void);
void UnloadRobotController(void);
std::optional<RobotStatusSnapshot> GetRobotStatusSnapshot(void);

#endif // ROBOT_CONTROLLER_H
