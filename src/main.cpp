#include "simulation/BlockingRobotManager.h"
#include "simulation/InputState.h"
#include "simulation/RobotController.h"
#include "simulation/map.h"
#include "ui/MapOverlay.h"
#include "ui/StatusOverlay.h"
#include "raylib.h"

int main(void) {
    constexpr int screenWidth = 1280;
    constexpr int screenHeight = 720;
    constexpr int targetFps = 60;

    InitWindow(screenWidth, screenHeight, "Robot Logistics Map Simulation");
    SetTargetFPS(targetFps);

    LogisticsMap logisticsMap;
    logisticsMap.init();

    BlockingRobotManager blockingRobotManager;
    blockingRobotManager.initBlockingRobots(logisticsMap);

    RobotController robotController(logisticsMap, blockingRobotManager);
    robotController.initialize();

    while (!WindowShouldClose()) {
        const float deltaTime = GetFrameTime();
        const InputState inputState = ReadInputState();

        blockingRobotManager.update(deltaTime);
        robotController.update(deltaTime, inputState);

        BeginDrawing();

        logisticsMap.draw();
        DrawMapOverlay();
        blockingRobotManager.draw();
        robotController.draw();
        DrawStatusOverlay(robotController.statusSnapshot());

        EndDrawing();
    }

    robotController.unload();
    blockingRobotManager.clear();
    logisticsMap.unload();
    CloseWindow();

    return 0;
}
