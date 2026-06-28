#pragma once

#include "rendering/RobotRenderer.h"
#include "simulation/ChargingManager.h"
#include "simulation/EmergencyStopController.h"
#include "robots/Robot.h"
#include "simulation/RouteFollower.h"
#include "simulation/InputState.h"
#include "simulation/RobotRoutePlanner.h"
#include "simulation/SimConfig.h"
#include "simulation/RobotStatusSnapshot.h"
#include "simulation/RobotTaskFlow.h"

#include <memory>
#include <optional>

class LogisticsMap;
class BlockingRobotManager;

class RobotController {
  public:
    RobotController(const LogisticsMap& logisticsMap,
                    const BlockingRobotManager& blockingRobotManager,
                    SimConfig simConfig = SimConfig::Default());

    void initialize();
    void update(float deltaTime, const InputState& inputState);
    void draw() const;
    void unload();
    std::optional<RobotStatusSnapshot> statusSnapshot() const;

  private:
    void startPickupTrip();
    void startDropoffTrip();
    void startChargingTrip();
    void startCharging();

    [[nodiscard]] bool updateStationaryTask(float deltaTime);
    void updatePickup(float deltaTime);
    void updateDropoff(float deltaTime);
    void updateCharging(float deltaTime);
    void updateWaypointTravel();

    const LogisticsMap& logisticsMap_;
    SimConfig simConfig_;
    RobotRoutePlanner routePlanner_;
    RouteFollower routeFollower_;
    ChargingManager chargingManager_;
    EmergencyStopController emergencyStopController_;
    RobotTaskFlow taskFlow_;
    RobotRenderer robotRenderer_;
    std::unique_ptr<Robot> robot_;
};
