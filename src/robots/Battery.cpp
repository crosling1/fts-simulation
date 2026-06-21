#include "robots/Battery.h"

#include <algorithm>

namespace {
constexpr float minimumChargePercentage = 0.0f;
constexpr float maximumChargePercentage = 100.0f;

float ClampChargePercentage(float percentage) {
    return std::clamp(percentage, minimumChargePercentage, maximumChargePercentage);
}
} // namespace

Battery::Battery(float chargePercentage) : chargePercentage_(ClampChargePercentage(chargePercentage)) {}

float Battery::getChargePercentage(void) const {
    return chargePercentage_;
}

bool Battery::isFull(void) const {
    return chargePercentage_ >= maximumChargePercentage;
}

bool Battery::isEmpty(void) const {
    return chargePercentage_ <= minimumChargePercentage;
}

bool Battery::isLow(float thresholdPercentage) const {
    return chargePercentage_ < thresholdPercentage;
}

void Battery::drain(float percentage) {
    chargePercentage_ = ClampChargePercentage(chargePercentage_ - percentage);
}

void Battery::charge(float percentage) {
    chargePercentage_ = ClampChargePercentage(chargePercentage_ + percentage);
}

void Battery::setChargePercentage(float percentage) {
    chargePercentage_ = ClampChargePercentage(percentage);
}
