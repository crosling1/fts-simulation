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
    void reset();

    [[nodiscard]] bool isPickingUp() const;
    [[nodiscard]] bool isDroppingOff() const;
    [[nodiscard]] bool isCharging() const;
    [[nodiscard]] bool isRoutingToPickup() const;
    [[nodiscard]] bool isRoutingToDropoff() const;
    [[nodiscard]] bool isRoutingToCharging() const;

    void startTripToPickup();
    void startTripToDropoff();
    void startTripToCharging();
    void startPickingUp();
    void startDroppingOff();
    void startCharging();

    [[nodiscard]] bool updatePickup(float deltaTime);
    [[nodiscard]] bool updateDropoff(float deltaTime);

  private:
    void enterPhase(RobotTaskPhase newPhase);

    RobotTaskPhase phase_ = RobotTaskPhase::ToPickup;
    float stateTimer_ = 0.0f;
};
