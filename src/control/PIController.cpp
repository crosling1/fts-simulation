#include "control/PIController.h"

#include <algorithm>

PIController::PIController(Config config) : config_(config) {}

float PIController::update(float error, float deltaTime, float maxOutput) {
    errorIntegral_ += error * deltaTime;
    errorIntegral_ =
        std::clamp(errorIntegral_, 0.0f, std::max(0.0f, config_.maxIntegralError));

    if (config_.proportionalGain <= 0.0f && config_.integralGain <= 0.0f) {
        return maxOutput;
    }

    const float output =
        (config_.proportionalGain * error) + (config_.integralGain * errorIntegral_);
    return std::clamp(output, 0.0f, maxOutput);
}

void PIController::reset() {
    errorIntegral_ = 0.0f;
}
