#include "simulation/RobotTaskFlow.h"

namespace {
constexpr float pickupDuration = 1.0f;
constexpr float dropoffDuration = 1.0f;
} // namespace

void RobotTaskFlow::reset(void) {
    phase_ = RobotTaskPhase::ToPickup;
    stateTimer_ = 0.0f;
}

RobotTaskPhase RobotTaskFlow::phase(void) const {
    return phase_;
}

bool RobotTaskFlow::isPickingUp(void) const {
    return phase_ == RobotTaskPhase::PickingUp;
}

bool RobotTaskFlow::isDroppingOff(void) const {
    return phase_ == RobotTaskPhase::DroppingOff;
}

bool RobotTaskFlow::isCharging(void) const {
    return phase_ == RobotTaskPhase::Charging;
}

bool RobotTaskFlow::isRoutingToPickup(void) const {
    return phase_ == RobotTaskPhase::ToPickup;
}

bool RobotTaskFlow::isRoutingToDropoff(void) const {
    return phase_ == RobotTaskPhase::ToDropoff;
}

bool RobotTaskFlow::isRoutingToCharging(void) const {
    return phase_ == RobotTaskPhase::ToCharging;
}

void RobotTaskFlow::startTripToPickup(void) {
    phase_ = RobotTaskPhase::ToPickup;
    stateTimer_ = 0.0f;
}

void RobotTaskFlow::startTripToDropoff(void) {
    phase_ = RobotTaskPhase::ToDropoff;
    stateTimer_ = 0.0f;
}

void RobotTaskFlow::startTripToCharging(void) {
    phase_ = RobotTaskPhase::ToCharging;
    stateTimer_ = 0.0f;
}

void RobotTaskFlow::startPickingUp(void) {
    phase_ = RobotTaskPhase::PickingUp;
    stateTimer_ = 0.0f;
}

void RobotTaskFlow::startDroppingOff(void) {
    phase_ = RobotTaskPhase::DroppingOff;
    stateTimer_ = 0.0f;
}

void RobotTaskFlow::startCharging(void) {
    phase_ = RobotTaskPhase::Charging;
    stateTimer_ = 0.0f;
}

bool RobotTaskFlow::updatePickup(float deltaTime) {
    stateTimer_ += deltaTime;
    return stateTimer_ >= pickupDuration;
}

bool RobotTaskFlow::updateDropoff(float deltaTime) {
    stateTimer_ += deltaTime;
    return stateTimer_ >= dropoffDuration;
}
