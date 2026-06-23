#pragma once

#include <cstdint>

enum class RobotTaskPhase : std::uint8_t {
    ToPickup,
    PickingUp,
    ToDropoff,
    DroppingOff,
    ToCharging,
    Charging,
};

class RobotTaskFlow {
  public:
    void reset(void);

    bool isPickingUp(void) const;
    bool isDroppingOff(void) const;
    bool isCharging(void) const;
    bool isRoutingToPickup(void) const;
    bool isRoutingToDropoff(void) const;
    bool isRoutingToCharging(void) const;

    void startTripToPickup(void);
    void startTripToDropoff(void);
    void startTripToCharging(void);
    void startPickingUp(void);
    void startDroppingOff(void);
    void startCharging(void);

    bool updatePickup(float deltaTime);
    bool updateDropoff(float deltaTime);

  private:
    RobotTaskPhase phase_ = RobotTaskPhase::ToPickup;
    float stateTimer_ = 0.0f;
};
