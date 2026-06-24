#pragma once

#include "chargingmanager.h"
#include "emergencystopcontroller.h"
#include "robots/Robot.h"
#include "routefollower.h"
#include "simulation/InputState.h"
#include "simulation/RobotRoutePlanner.h"
#include "simulation/RobotStatusSnapshot.h"
#include "simulation/RobotTaskFlow.h"

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
    void startPickupTrip();
    void startDropoffTrip();
    void startChargingTrip();
    void updatePickup(float deltaTime);
    void updateDropoff(float deltaTime);
    void updateCharging(float deltaTime);
    void updateWaypointTravel();

    const LogisticsMap& logisticsMap_;
    RobotRoutePlanner routePlanner_;
    RouteFollower routeFollower_;
    ChargingManager chargingManager_;
    EmergencyStopController emergencyStopController_;
    RobotTaskFlow taskFlow_;
    std::unique_ptr<Robot> robot_;
};
