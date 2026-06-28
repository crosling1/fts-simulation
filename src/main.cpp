#include "simulation/BlockingRobotManager.h"
#include "simulation/InputState.h"
#include "simulation/RobotController.h"
#include "simulation/SimConfig.h"
#include "simulation/map.h"
#include "ui/MapOverlay.h"
#include "ui/StatusOverlay.h"
#include "raylib.h"

#include <exception>
#include <iostream>

int main() try {
    constexpr int screenWidth = 1280;
    constexpr int screenHeight = 720;
    constexpr int targetFps = 60;

    InitWindow(screenWidth, screenHeight, "Robot Logistics Map Simulation");
    SetTargetFPS(targetFps);

    LogisticsMap logisticsMap;
    logisticsMap.init();

    const SimConfig simConfig = SimConfig::Default();

    BlockingRobotManager blockingRobotManager(simConfig);
    blockingRobotManager.initBlockingRobots(logisticsMap);

    RobotController robotController(logisticsMap, blockingRobotManager, simConfig);
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
        DrawStatusOverlay(robotController.statusSnapshot(), simConfig);

        EndDrawing();
    }

    robotController.unload();
    blockingRobotManager.clear();
    logisticsMap.unload();
    CloseWindow();

    return 0;
} catch (const std::exception& e) {
    std::cerr << "Fatal error: " << e.what() << '\n';
    return 1;
} catch (...) {
    std::cerr << "Fatal error: unknown exception\n";
    return 1;
}
