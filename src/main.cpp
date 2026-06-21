#include "simulation/BlockingRobotController.h"
#include "simulation/RobotController.h"
#include "simulation/map.h"
#include "ui/StatusOverlay.h"
#include "raylib.h"

int main(void) {
    const int screenWidth = 1280;
    const int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "Robot Logistics Map Simulation");
    SetTargetFPS(60);

    InitMap();
    InitBlockingRobotController();
    InitRobotController();

    while (!WindowShouldClose()) {
        UpdateBlockingRobotController();
        UpdateRobotController();

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
