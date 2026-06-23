#pragma once

struct InputState {
    bool emergencyStopPressed = false;
    bool resetEmergencyStopPressed = false;
};

InputState ReadInputState();
