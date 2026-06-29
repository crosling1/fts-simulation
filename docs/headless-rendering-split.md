# Headless Core And Rendering Split Notes

This document records the current source layout for the split between the core
simulation library and a raylib-backed rendering layer. The current `fts_core`
target still exposes raylib types such as `Vector2` and `Rectangle` in public
headers, so raylib remains a public dependency of the core for now.

## Current CMake Boundary

- `fts_core` is the reusable library linked by `robot_sim` and `unit_tests`.
- `fts_rendering` contains rendering-only files from `src/rendering` and
  raylib-dependent UI overlay files from `src/ui`.
- `fts_rendering` depends on `fts_core` and raylib.
- `robot_sim` owns the raylib window loop in `src/main.cpp`.
- `robot_sim` links both `fts_core` and `fts_rendering`.
- `unit_tests` link against `fts_core`, so test-facing simulation APIs should
  stay available from the core target.
- `src/main.cpp` should remain an application shell, even after a rendering
  target exists. It currently owns `InitWindow`, `BeginDrawing`, `EndDrawing`
  and `CloseWindow`.

## Headless Core Candidates

These files contain simulation, navigation, robot, control, sensor or data
loading behavior without direct raylib drawing or input calls. Many still use
raylib data types in their public APIs; those types should remain in place until
the project introduces project-owned geometry types.

- `src/control/PIController.cpp`
- `src/robots/Battery.cpp`
- `src/robots/Robot.cpp`
- `src/robots/WorkerRobot.cpp`
- `src/sensors/ProximitySensor.cpp`
- `src/simulation/ChargingManager.cpp`
- `src/simulation/EmergencyStopController.cpp`
- `src/simulation/MapData.cpp`
- `src/simulation/NavigationGraph.cpp`
- `src/simulation/RobotController.cpp`
- `src/simulation/RobotRoutePlanner.cpp`
- `src/simulation/RobotTaskFlow.cpp`
- `src/simulation/navigation.cpp`

Related public headers:

- `include/control/PIController.h`
- `include/robots/Battery.h`
- `include/robots/Robot.h`
- `include/robots/RobotRenderData.h`
- `include/robots/RobotState.h`
- `include/robots/WorkerRobot.h`
- `include/sensors/ProximitySensor.h`
- `include/sensors/Sensor.h`
- `include/simulation/ChargingManager.h`
- `include/simulation/EmergencyStopController.h`
- `include/simulation/ILogisticsMap.h`
- `include/simulation/MapData.h`
- `include/simulation/MathUtils.h`
- `include/simulation/NavigationGraph.h`
- `include/simulation/RobotController.h`
- `include/simulation/RobotRoutePlanner.h`
- `include/simulation/RobotStatusSnapshot.h`
- `include/simulation/RobotTaskFlow.h`
- `include/simulation/SimConfig.h`
- `include/simulation/navigation.h`

## Rendering Target

These files are built by `fts_rendering` because they directly own raylib
drawing calls.

- `src/rendering/RobotRenderer.cpp`
- `src/ui/MapOverlay.cpp`
- `src/ui/StatusOverlay.cpp`

Related public headers:

- `include/rendering/RobotRenderer.h`
- `include/ui/MapOverlay.h`
- `include/ui/StatusOverlay.h`

## Raylib Boundary Candidates

These files are still in simulation directories, but they mix core state or
queries with raylib drawing, input or collision helpers. They are the likely
first extraction points before `fts_core` can become truly headless.

- `src/simulation/map.cpp`
  - Core behavior: map-data ownership, dock/station queries, road clamping and
    navigation-node access.
  - raylib behavior: `ClearBackground`, map grid/road/warehouse/station drawing
    and collision helper usage.
- `src/simulation/BlockingRobotManager.cpp`
  - Core behavior: blocking robot path state, movement and proximity queries.
  - raylib behavior: blocking robot drawing and circle collision helper usage.
- `src/simulation/RouteFollower.cpp`
  - Core behavior: active waypoint management and road-constrained movement.
  - raylib behavior: active path drawing.
- `src/simulation/InputState.cpp`
  - Core behavior: normalized `InputState` data.
  - raylib behavior: keyboard polling with `IsKeyPressed`.

Related public headers:

- `include/simulation/map.h`
- `include/simulation/BlockingRobotManager.h`
- `include/simulation/RouteFollower.h`
- `include/simulation/InputState.h`

## Suggested Future Steps

1. Introduce project-owned geometry types for positions and rectangles.
2. Keep compatibility adapters for raylib `Vector2` and `Rectangle` while
   migrating public headers.
3. Move drawing methods such as `LogisticsMap::draw`,
   `BlockingRobotManager::draw` and `RouteFollower::draw` into rendering-side
   adapters.
4. Move `ReadInputState` behind an input adapter owned by the app or rendering
   layer.
5. After public headers no longer force `fts_core` to expose raylib as a public
   dependency, make raylib private to the rendering and app-side targets.
