#pragma once

namespace SimConstants {
inline constexpr float kBatteryDrainPerPixel = 0.01f;
inline constexpr float kChargeRatePerSecond = 10.0f;
inline constexpr float kPickupDurationSeconds = 1.0f;
inline constexpr float kDropoffDurationSeconds = 1.0f;
inline constexpr float kReachedDistance = 2.0f;
inline constexpr float kChargeAfterDropoffThreshold = 10.0f;
inline constexpr float kMinimumBatteryAfterJob = 10.0f;
inline constexpr float kBlockingRobotRadius = 14.0f;
inline constexpr float kBlockingRobotSpeed = 65.0f;
} // namespace SimConstants