#pragma once

#include <cstdint>

enum class RobotState : std::uint8_t {
    Idle,
    Moving,
    PickingUp,
    CarryingItem,
    DroppingOff,
    Arrived,
    BatteryDepleted,
    Charging,
};
