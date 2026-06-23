#ifndef ROBOT_CONTROLLER_H
#define ROBOT_CONTROLLER_H

#include "robots/Robot.h"
#include "simulation/InputState.h"
#include "simulation/RobotRoutePlanner.h"
#include "simulation/RobotStatusSnapshot.h"
#include "simulation/RobotTaskFlow.h"

#include "raylib.h"

#include <cstddef>
#include <memory>
#include <optional>
#include <vector>

class LogisticsMap;
class BlockingRobotManager;

class RobotController {
  public:
    RobotController(const LogisticsMap& logisticsMap,
                    const BlockingRobotManager& blockingRobotManager);

    void initialize();
    void update(float deltaTime, const InputState& inputState);
    void draw() const;
    void unload();
    std::optional<RobotStatusSnapshot> statusSnapshot() const;

  private:
    Vector2 getRobotPosition() const;
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
    const BlockingRobotManager& blockingRobotManager_;
    RobotRoutePlanner routePlanner_;
    RobotTaskFlow taskFlow_;
    std::unique_ptr<Robot> robot_;
    std::vector<Vector2> activePath_;
    Vector2 activePathStart_ = {0.0f, 0.0f};
    std::size_t currentWaypointIndex_ = 0;
    bool emergencyStopActive_ = false;
    Robot::State stateBeforeEmergencyStop_ = Robot::State::Idle;
};

#endif // ROBOT_CONTROLLER_H
