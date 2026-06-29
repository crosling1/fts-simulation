#include "simulation/InputState.h"

#include "raylib.h"

InputState ReadInputState() {
    InputState inputState;
    inputState.emergencyStopPressed = IsKeyPressed(KEY_E);
    inputState.resetEmergencyStopPressed = IsKeyPressed(KEY_R);

    return inputState;
}
