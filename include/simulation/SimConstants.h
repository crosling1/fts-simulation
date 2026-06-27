#pragma once

namespace SimConstants {

namespace Battery {
inline constexpr float kDrainPerPixel = 0.01f;
inline constexpr float kChargeRatePerSecond = 10.0f;
inline constexpr float kChargeAfterDropoffThreshold = 10.0f;
inline constexpr float kMinimumAfterJob = 10.0f;
} // namespace Battery

namespace Task {
inline constexpr float kPickupDurationSeconds = 1.0f;
inline constexpr float kDropoffDurationSeconds = 1.0f;
} // namespace Task

namespace Navigation {
inline constexpr float kReachedDistance = 2.0f;
} // namespace Navigation

namespace BlockingRobot {
inline constexpr float kRadius = 14.0f;
inline constexpr float kSpeed = 65.0f;
} // namespace BlockingRobot

} // namespace SimConstants