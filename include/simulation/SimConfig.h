#pragma once

struct SimConfig {
    float robotSpeed = 120.0f;
    float robotRotationSpeed = 180.0f;
    float robotSize = 16.0f;
    float robotProportionalGain = 2.0f;
    float robotIntegralGain = 0.25f;
    float robotMaxIntegralError = 200.0f;

    float batteryDrainPerPixel = 0.01f;
    float batteryChargeRatePerSecond = 10.0f;
    float lowBatteryThreshold = 10.0f;
    float batteryWarningThreshold = 30.0f;
    float emergencyBatteryThreshold = 10.0f;

    float pickupDurationSeconds = 1.0f;
    float dropoffDurationSeconds = 1.0f;
    float reachedDistance = 2.0f;
    float sensorRangeMultiplier = 2.0f;

    float blockingRobotRadius = 14.0f;
    float blockingRobotSpeed = 65.0f;

    [[nodiscard]] static const SimConfig& Default() {
        static const SimConfig defaultConfig{};
        return defaultConfig;
    }
};
