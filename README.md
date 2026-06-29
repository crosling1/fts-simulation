# FTS Simulation

FTS Simulation is a C++17 and raylib-based logistics robot simulation inspired by
Automated Guided Vehicle (AGV) and Fahrerloses Transportsystem (FTS) workflows.

The project simulates a warehouse environment containing storage locations,
pickup and delivery stations, charging infrastructure, road-network navigation
and moving robots.

A main robot repeatedly performs pickup and delivery tasks while remaining
constrained to the road network. Routes are generated using A* pathfinding,
movement is controlled by a PI controller, battery consumption is tracked during
operation, and charging decisions are made automatically when future tasks would
leave insufficient remaining battery capacity.

The simulation also includes moving blocking robots, proximity-based obstacle
handling, emergency stop controls, runtime status monitoring, automated testing
and continuous integration.

The project serves both as a robotics simulation exercise and as a portfolio
project focused on modern C++ design, maintainability and responsibility-based
software architecture.

## Features

### Navigation

* A* road-network pathfinding
* Waypoint-based route following
* Road-constrained movement
* Route optimization by removing unnecessary pass-through nodes
* Route visualization

### Robot Operation

* Continuous pickup and delivery task loop
* Task phase management
* Item carrying visualization
* PI-controlled movement
* Explicit robot state machine

### Energy Management

* Distance-based battery consumption
* Battery-aware task planning
* Automatic charging decisions
* Charging station navigation
* Recharge cycle management

### Safety And Obstacle Handling

* Radius-based proximity checks
* Dynamic blocking robots
* Automatic obstacle waiting behavior
* Emergency stop control
* Runtime reset support

### Development Tooling

* Catch2 unit testing
* clang-format integration
* clang-tidy integration
* GitHub Actions CI pipeline
* CMake build system
* Makefile convenience commands

## Architecture

The simulation follows a responsibility-based architecture. `RobotController`
coordinates the core robot workflow, while focused components handle navigation,
route following, task progression, charging decisions and safety control.
Environment simulation, rendering and UI overlays stay outside the core control
logic.

```text
main.cpp / Simulation Loop
      |
+-- Environment / Dynamic Obstacle Simulation
|     |
|     +-- LogisticsMap
|     |     +-- ILogisticsMap
|     |     +-- MapData
|     |
|     +-- BlockingRobotManager
|           +-- BlockingRobot
|
+-- Core Simulation Logic
|     |
|     +-- RobotController
|           |
|           +-- Navigation
|           |     +-- RobotRoutePlanner
|           |     +-- NavigationGraph
|           |     +-- RouteFollower
|           |
|           +-- Task Flow
|           |     +-- RobotTaskFlow
|           |
|           +-- Energy Management
|           |     +-- ChargingManager
|           |
|           +-- Safety / Control
|           |     +-- EmergencyStopController
|           |
|           +-- Robot Model
|                 +-- WorkerRobot
|                 +-- Robot
|                 +-- Battery
|                 +-- ProximitySensor
|
+-- Rendering And UI
      |
      +-- LogisticsMap::draw
      +-- BlockingRobotManager::draw
      +-- RobotRenderer
      +-- MapOverlay
      +-- StatusOverlay
```

## Core Areas

| Area                         | Responsibility                                                        |
| ---------------------------- | --------------------------------------------------------------------- |
| `Robot` / `WorkerRobot`      | Robot motion, state transitions, target handling and robot identity.  |
| `Battery`                    | Battery charge state, drain, charging and threshold checks.           |
| `RobotController`            | Coordinates task flow, routing, movement, charging and safety control. |
| `RouteFollower`              | Applies active waypoint paths and keeps robot movement road-constrained. |
| `RobotRoutePlanner`          | Builds pickup, dropoff and charging routes from map navigation data.  |
| `NavigationGraph`            | Cached road-segment validity, weighted adjacency and A* pathfinding.  |
| `ChargingManager`            | Battery-aware charging decision logic.                                |
| `EmergencyStopController`    | Emergency-stop toggling and obstacle-pause decisions.                 |
| `ProximitySensor`            | Radius-based proximity checks against active blocking robots.         |
| `BlockingRobot`              | Individual blocking robot position, path following and proximity data. |
| `BlockingRobotManager`       | Dynamic obstacle collection, initialization, movement updates and queries. |
| `RobotTaskFlow`              | Pickup, dropoff and charging phase progression.                       |
| `RobotRenderer`              | raylib rendering for robot visuals, scan radius and carried items.    |
| `MapOverlay` / `StatusOverlay` | UI overlays for map labels and runtime robot status.                |
| `SimConfig`                  | Shared configuration for movement, battery, timing and blocking robots. |
| Development Tooling          | Catch2 tests, formatting, linting and CI automation.                  |

## Runtime Configuration

`SimConfig` centralizes simulation tuning values. The current fields are:

| Field | Purpose |
| ----- | ------- |
| `robotSpeed` | Maximum robot travel speed. |
| `robotRotationSpeed` | Maximum robot rotation speed in degrees per second. |
| `robotSize` | Robot render size and base sensor-size input. |
| `robotProportionalGain` | PI controller proportional gain for movement speed. |
| `robotIntegralGain` | PI controller integral gain for movement speed. |
| `robotMaxIntegralError` | PI controller integral clamp. |
| `batteryDrainPerPixel` | Battery drain per traveled pixel. |
| `batteryChargeRatePerSecond` | Charging rate while the robot is charging. |
| `lowBatteryThreshold` | Battery threshold for charging decisions. |
| `batteryWarningThreshold` | Battery threshold for warning-colored UI. |
| `emergencyBatteryThreshold` | Minimum reserve used for future-task battery checks. |
| `pickupDurationSeconds` | Simulated pickup duration. |
| `dropoffDurationSeconds` | Simulated dropoff duration. |
| `reachedDistance` | Distance tolerance for reaching targets and path nodes. |
| `sensorRangeMultiplier` | Multiplier applied to robot size for proximity range. |
| `blockingRobotRadius` | Default collision radius for blocking robots. |
| `blockingRobotSpeed` | Default blocking-robot speed before path multipliers. |

## Code Organization

The codebase keeps runtime behavior split into focused modules:

* `SimConfig` owns shared tuning values for movement, battery, timing and
  blocking robots.
* `RobotRenderer` owns robot drawing, while `Robot` owns motion, state and
  battery behavior.
* `RobotRoutePlanner` builds routes and `RouteFollower` executes active
  waypoint paths.
* `BlockingRobot` stores per-robot path state, while `BlockingRobotManager`
  manages the collection and proximity queries.
* `NavigationGraph` builds road-segment validity and weighted adjacency once
  for reuse by route requests.
* `MapData` loads JSON map data and `ILogisticsMap` exposes the navigation
  interface consumed by route planning.
* Production code is built as the reusable `fts_core` CMake library, shared by
  the application and Catch2 unit tests.

## Design Decisions

### Explicit Ownership

The simulation avoids hidden global state. Main application objects are created
and owned explicitly by `main.cpp`. Runtime configuration is owned at the
application level and passed to dependent components as a read-only reference.

### Controller As Coordinator

`RobotController` coordinates the robot workflow but does not contain every
behavior directly. Navigation, charging and safety decisions are delegated to
dedicated components.

### Separation Of Responsibilities

Navigation, task management, charging logic and safety behavior are separated
into focused modules. This reduces coupling and makes future changes easier.

### Road-Constrained Navigation

Robots operate exclusively on the road network. Route generation uses A*
pathfinding over a cached navigation graph and road validation to ensure
realistic movement behavior.

### Extensibility

The architecture is designed to support additional robot types, sensors,
navigation strategies and workflow logic without requiring major changes to the
central controller.

## Project Structure

* `include/` Public interfaces and class declarations.
* `src/` Simulation, navigation, control and application logic.
* `tests/` Unit tests and shared test utilities.
* `assets/` Runtime assets and project resources.
* `.github/` Continuous integration workflows.
* `tools/` Development and maintenance utilities.

The codebase is organized by responsibility rather than by large monolithic
classes. New functionality is typically added to an existing architectural area
instead of expanding a single controller.
