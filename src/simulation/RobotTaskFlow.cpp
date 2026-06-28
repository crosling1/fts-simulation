#include "simulation/RobotTaskFlow.h"

RobotTaskFlow::RobotTaskFlow(const SimConfig& simConfig) : simConfig_(simConfig) {}

void RobotTaskFlow::enterPhase(RobotTaskPhase newPhase) {
    phase_ = newPhase;
    stateTimer_ = 0.0f;
}

void RobotTaskFlow::reset() {
    phase_ = RobotTaskPhase::ToPickup;
    stateTimer_ = 0.0f;
}

[[nodiscard]] bool RobotTaskFlow::isPickingUp() const {
    return phase_ == RobotTaskPhase::PickingUp;
}

[[nodiscard]] bool RobotTaskFlow::isDroppingOff() const {
    return phase_ == RobotTaskPhase::DroppingOff;
}

[[nodiscard]] bool RobotTaskFlow::isCharging() const {
    return phase_ == RobotTaskPhase::Charging;
}

[[nodiscard]] bool RobotTaskFlow::isRoutingToPickup() const {
    return phase_ == RobotTaskPhase::ToPickup;
}

[[nodiscard]] bool RobotTaskFlow::isRoutingToDropoff() const {
    return phase_ == RobotTaskPhase::ToDropoff;
}

[[nodiscard]] bool RobotTaskFlow::isRoutingToCharging() const {
    return phase_ == RobotTaskPhase::ToCharging;
}

void RobotTaskFlow::startTripToPickup() {
    enterPhase(RobotTaskPhase::ToPickup);
}
void RobotTaskFlow::startTripToDropoff() {
    enterPhase(RobotTaskPhase::ToDropoff);
}
void RobotTaskFlow::startTripToCharging() {
    enterPhase(RobotTaskPhase::ToCharging);
}
void RobotTaskFlow::startPickingUp() {
    enterPhase(RobotTaskPhase::PickingUp);
}
void RobotTaskFlow::startDroppingOff() {
    enterPhase(RobotTaskPhase::DroppingOff);
}
void RobotTaskFlow::startCharging() {
    enterPhase(RobotTaskPhase::Charging);
}

[[nodiscard]] bool RobotTaskFlow::updatePickup(float deltaTime) {
    stateTimer_ += deltaTime;
    return stateTimer_ >= simConfig_.pickupDurationSeconds;
}

[[nodiscard]] bool RobotTaskFlow::updateDropoff(float deltaTime) {
    stateTimer_ += deltaTime;
    return stateTimer_ >= simConfig_.dropoffDurationSeconds;
}
