#include "robots/Battery.h"

#include <algorithm>

namespace {
constexpr float minimumChargePercentage = 0.0f;
constexpr float maximumChargePercentage = 100.0f;

float ClampChargePercentage(float percentage) noexcept {
    return std::clamp(percentage, minimumChargePercentage, maximumChargePercentage);
}
} // namespace

Battery::Battery(float chargePercentage)
    : chargePercentage_(
          std::clamp(chargePercentage, minimumChargePercentage, maximumChargePercentage)) {}

float Battery::getChargePercentage() const noexcept {
    return chargePercentage_;
}

bool Battery::isFull() const noexcept {
    return chargePercentage_ >= maximumChargePercentage;
}

bool Battery::isEmpty() const noexcept {
    return chargePercentage_ <= minimumChargePercentage;
}

bool Battery::isLow(float thresholdPercentage) const noexcept {
    return chargePercentage_ < thresholdPercentage;
}

void Battery::drain(float percentage) noexcept {
    chargePercentage_ = ClampChargePercentage(chargePercentage_ - percentage);
}

void Battery::charge(float percentage) noexcept {
    chargePercentage_ = ClampChargePercentage(chargePercentage_ + percentage);
}

void Battery::setChargePercentage(float percentage) noexcept {
    chargePercentage_ = ClampChargePercentage(percentage);
}
