# Headless Core And Rendering Split Notes

This document records the current source layout for the split between the core
simulation library and a raylib-backed rendering layer. Core public headers use
project-owned geometry types from `include/simulation/Geometry.h`; raylib
geometry remains behind rendering and adapter boundaries.

## Current CMake Boundary

- `fts_core` is the reusable library linked by `robot_sim` and `unit_tests`.
- `fts_core` uses `Vec2` and `Rect` in simulation-facing public APIs.
- `fts_core` does not link raylib.
- `fts_rendering` contains rendering-only files from `src/rendering` and
  raylib-dependent UI overlay files from `src/ui`, plus raylib implementations
  for draw/input methods declared on core types.
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
loading behavior without direct raylib drawing or input calls.

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
- `include/simulation/Geometry.h`
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

- `include/rendering/RaylibGeometry.h`
- `include/rendering/RobotRenderer.h`
- `include/ui/MapOverlay.h`
- `include/ui/StatusOverlay.h`

## Raylib Boundary Candidates

These declarations are still in simulation headers for compatibility, but their
raylib-backed implementations live in `fts_rendering`.

- `LogisticsMap::draw`
  - Core behavior: map-data ownership, dock/station queries, road clamping and
    navigation-node access.
  - raylib implementation: `src/rendering/LogisticsMapRenderer.cpp`.
- `BlockingRobotManager::draw`
  - Core behavior: blocking robot path state, movement and proximity queries.
  - raylib implementation: `src/rendering/BlockingRobotRenderer.cpp`.
- `RouteFollower::draw`
  - Core behavior: active waypoint management and road-constrained movement.
  - raylib implementation: `src/rendering/RouteFollowerRenderer.cpp`.
- `ReadInputState`
  - Core behavior: normalized `InputState` data.
  - raylib implementation: `src/rendering/RaylibInputState.cpp`.

Related public headers:

- `include/simulation/map.h`
- `include/simulation/BlockingRobotManager.h`
- `include/simulation/RouteFollower.h`
- `include/simulation/InputState.h`

## Suggested Future Steps

1. Replace compatibility draw/input declarations on core types with dedicated
   rendering and input adapter interfaces.
2. Keep `unit_tests` linked against `fts_core` only so the headless boundary
   stays visible in CI.
