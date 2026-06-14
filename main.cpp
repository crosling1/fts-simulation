#include "raylib.h"
#include "RobotController.h"
#include "map.h"

int main(void) {
    const int screenWidth = 1280;
    const int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "Robot Logistics Map Simulation");
    SetTargetFPS(60);

    InitMap();
    InitRobotController();

    while (!WindowShouldClose()) {
        UpdateRobotController();

        BeginDrawing();

        DrawMap();
        DrawRobotController();

        EndDrawing();
    }

    UnloadRobotController();
    UnloadMap();
    CloseWindow();

    return 0;
}
