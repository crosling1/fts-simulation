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
coordinates the runtime workflow, while focused components handle navigation,
route following, task progression, charging decisions, obstacle checks,
rendering and status display.

```text
Simulation Loop
      |
      v
RobotController
      |
      +-- Navigation Layer
      |      |
      |      +-- RobotRoutePlanner
      |      +-- NavigationGraph
      |      +-- RouteFollower
      |
      +-- Task Layer
      |      |
      |      +-- RobotTaskFlow
      |
      +-- Energy Layer
      |      |
      |      +-- ChargingManager
      |
      +-- Safety Layer
      |      |
      |      +-- EmergencyStopController
      |      +-- BlockingRobotManager
      |
      v
Robot
      |
      +-- WorkerRobot
      +-- Battery
      +-- ProximitySensor

Environment
      |
      +-- LogisticsMap
      +-- BlockingRobotManager
      |
      +-- BlockingRobot

Rendering And UI
      |
      +-- RobotRenderer
      +-- MapOverlay
      +-- StatusOverlay
```

## Core Areas

| Area                         | Responsibility                                                        |
| ---------------------------- | --------------------------------------------------------------------- |
| `Robot` / `WorkerRobot`      | Robot motion, state transitions, target handling and robot identity.  |
| `Battery`                    | Battery charge state, drain, charging and threshold checks.           |
| `RobotController`            | Coordinates task flow, routing, movement, charging, safety and drawing. |
| `RouteFollower`              | Applies active waypoint paths and keeps robot movement road-constrained. |
| `RobotRoutePlanner`          | Builds pickup, dropoff and charging routes from map navigation data.  |
| `NavigationGraph`            | Cached road-segment validity, weighted adjacency and A* pathfinding.  |
| `ChargingManager`            | Battery-aware charging decision logic.                                |
| `EmergencyStopController`    | Emergency-stop toggling and obstacle-pause decisions.                 |
| `ProximitySensor`            | Radius-based proximity checks against active blocking robots.         |
| `BlockingRobot`              | Individual blocking robot position, path following and proximity data. |
| `BlockingRobotManager`       | Blocking robot collection, initialization, updates and queries.       |
| `RobotTaskFlow`              | Pickup, dropoff and charging phase progression.                       |
| `RobotRenderer`              | raylib rendering for robot visuals, scan radius and carried items.    |
| `MapOverlay` / `StatusOverlay` | UI overlays for map labels and runtime robot status.                |
| `SimConfig`                  | Shared configuration for movement, battery, timing and blocking robots. |
| Development Tooling          | Catch2 tests, formatting, linting and CI automation.                  |

## Code Quality Improvements

Recent refactoring work focused on reducing coupling and making the codebase
easier to review and extend:

* Centralized simulation tuning values in `SimConfig`, covering movement,
  battery, timing and blocking-robot settings.
* Added `[[nodiscard]]`, `noexcept` and modern C++ empty parameter lists where
  appropriate.
* Made `ChargingManager` query functions const-correct.
* Replaced one-off `printType()` behavior with reusable `typeName()` data.
* Extracted robot drawing from `Robot` into `RobotRenderer`.
* Split route execution into `RouteFollower` and route construction into
  `RobotRoutePlanner`.
* Moved per-robot blocking behavior into `BlockingRobot`, keeping
  `BlockingRobotManager` focused on collection management.
* Optimized A* pathfinding with `NavigationGraph`, which builds the adjacency
  list once and reuses it for route requests.
* Kept JSON map loading local to `MapData` while exposing stable map data
  structures to the rest of the simulation.
* Added top-level exception handling so fatal startup errors are reported
  clearly before the application exits.
* Migrated unit tests from a custom expectation runner to Catch2.
* Split production code into the reusable `fts_core` CMake library, shared by
  the application and unit tests.

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
