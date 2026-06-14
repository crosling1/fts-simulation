#include "raylib.h"
#include "map.h"

int main(void) {
    const int screenWidth = 1280;
    const int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "Robot Logistics Map Simulation");
    SetTargetFPS(60);

    InitMap();

    while (!WindowShouldClose()) {
        BeginDrawing();

        DrawMap();

        EndDrawing();
    }

    UnloadMap();
    CloseWindow();

    return 0;
}
