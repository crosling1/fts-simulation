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

The simulation follows a responsibility-based architecture where the main
controller coordinates the overall workflow while specialized components handle
navigation, task progression, charging decisions and safety behavior.

```text
Simulation Loop
      |
      v
RobotController
      |
      +-- Navigation Layer
      |      |
      |      +-- RobotRoutePlanner
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
      |
      v
Robot
      |
      +-- Battery
      +-- ProximitySensor

Environment
      |
      +-- LogisticsMap
      +-- BlockingRobotManager
      |
      +-- BlockingRobot

UI
      |
      +-- StatusOverlay
      +-- RobotRenderer
```

## Core Areas

| Area                         | Responsibility                                                        |
| ---------------------------- | --------------------------------------------------------------------- |
| `Robot`                      | Movement, battery ownership, state transitions and target handling.   |
| `RobotRenderer`              | raylib rendering for robot visuals and carried-item display.          |
| `BlockingRobot`              | Individual blocking robot position, path following and proximity data. |
| `BlockingRobotManager`       | Blocking robot collection, initialization, updates and queries.       |
| `RobotTaskFlow`              | Pickup, dropoff and charging phase progression.                       |
| `ChargingManager`            | Battery-aware charging decision logic.                                |
| Navigation                   | A* pathfinding over map nodes using an adjacency list.                |
| `SimConstants`               | Shared simulation constants for timing, battery and movement values.  |
| `RobotController`            | Coordinates workflow by delegating to the focused components above.   |
| Development Tooling          | Catch2 tests, formatting, linting and CI automation.                  |

## Code Quality Improvements

Recent refactoring work focused on reducing coupling and making the codebase
easier to review and extend:

* Centralized duplicated simulation values in `SimConstants`.
* Added `[[nodiscard]]`, `noexcept` and modern C++ empty parameter lists where
  appropriate.
* Made `ChargingManager` query functions const-correct.
* Replaced one-off `printType()` behavior with reusable `typeName()` data.
* Extracted robot drawing from `Robot` into `RobotRenderer`.
* Moved per-robot blocking behavior into `BlockingRobot`, keeping
  `BlockingRobotManager` focused on collection management.
* Optimized A* pathfinding by building an adjacency list from navigation edges.
* Migrated unit tests from a custom expectation runner to Catch2.
* Split production code into the reusable `fts_core` CMake library, shared by
  the application and unit tests.

## Design Decisions

### Explicit Ownership

The simulation avoids hidden global state. Main application objects are created
and owned explicitly by `main.cpp`.

### Controller As Coordinator

`RobotController` coordinates the robot workflow but does not contain every
behavior directly. Navigation, charging and safety decisions are delegated to
dedicated components.

### Separation Of Responsibilities

Navigation, task management, charging logic and safety behavior are separated
into focused modules. This reduces coupling and makes future changes easier.

### Road-Constrained Navigation

Robots operate exclusively on the road network. Route generation uses A*
pathfinding over an adjacency list and road validation to ensure realistic
movement behavior.

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
