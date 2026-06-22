#include "control/PIController.h"

#include <algorithm>

PIController::PIController() : PIController(Config{}) {}

PIController::PIController(Config config) : config_(config) {}

float PIController::update(UpdateInput input) {
    errorIntegral_ += input.error * input.deltaTime;
    errorIntegral_ = std::clamp(errorIntegral_, 0.0f, std::max(0.0f, config_.maxIntegralError));

    if (config_.proportionalGain <= 0.0f && config_.integralGain <= 0.0f) {
        return input.maxOutput;
    }

    const float output =
        (config_.proportionalGain * input.error) + (config_.integralGain * errorIntegral_);
    return std::clamp(output, 0.0f, input.maxOutput);
}

void PIController::reset() {
    errorIntegral_ = 0.0f;
}
