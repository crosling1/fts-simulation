#pragma once

#include "robots/Robot.h"
#include "routefollower.h"
#include "simulation/InputState.h"
#include "simulation/RobotRoutePlanner.h"
#include "simulation/RobotStatusSnapshot.h"
#include "simulation/RobotTaskFlow.h"

#include "raylib.h"

#include <memory>
#include <optional>

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
    bool shouldChargeAtOrBelow(float thresholdPercentage) const;
    void startPickupTrip();
    void startDropoffTrip();
    void startChargingTrip();
    bool canCompleteNextDeliveryBeforeMinimumBattery() const;
    void updatePickup(float deltaTime);
    void updateDropoff(float deltaTime);
    void updateCharging(float deltaTime);
    void updateWaypointTravel();
    void updateEmergencyStop(const InputState& inputState);

    const LogisticsMap& logisticsMap_;
    const BlockingRobotManager& blockingRobotManager_;
    RobotRoutePlanner routePlanner_;
    RouteFollower routeFollower_;
    RobotTaskFlow taskFlow_;
    std::unique_ptr<Robot> robot_;
    bool emergencyStopActive_ = false;
    Robot::State stateBeforeEmergencyStop_ = Robot::State::Idle;
};
