#include "simulation/BlockingRobotController.h"
#include "simulation/InputState.h"
#include "simulation/RobotController.h"
#include "simulation/map.h"
#include "ui/StatusOverlay.h"
#include "raylib.h"

int main(void) {
    constexpr int screenWidth = 1280;
    constexpr int screenHeight = 720;
    constexpr int targetFps = 60;

    InitWindow(screenWidth, screenHeight, "Robot Logistics Map Simulation");
    SetTargetFPS(targetFps);

    InitMap();
    InitBlockingRobotController();
    InitRobotController();

    while (!WindowShouldClose()) {
        const float deltaTime = GetFrameTime();
        const InputState inputState = ReadInputState();

        UpdateBlockingRobotController();
        UpdateRobotController(deltaTime, inputState);

        BeginDrawing();

        DrawMap();
        DrawBlockingRobotController();
        DrawRobotController();
        DrawStatusOverlay(GetRobotStatusSnapshot());

        EndDrawing();
    }

    UnloadRobotController();
    UnloadBlockingRobotController();
    UnloadMap();
    CloseWindow();

    return 0;
}
