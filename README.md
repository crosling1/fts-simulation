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

* Unit testing
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
```

## Core Areas

| Area                | Responsibility                                                             |
| ------------------- | -------------------------------------------------------------------------- |
| Controller Layer    | Coordinates the overall robot workflow and delegates specialized behavior. |
| Navigation Layer    | Route planning, waypoint following and road-network movement.              |
| Task Layer          | Pickup, dropoff and charging phase management.                             |
| Energy Layer        | Battery monitoring and charging decisions.                                 |
| Safety Layer        | Emergency stop handling and obstacle pause checks.                         |
| Robot Domain        | Robot movement, state, battery and sensor ownership.                       |
| Environment         | Map data, road network, charging stations and blocking robots.             |
| UI Layer            | Runtime overlays, visualization and simulation feedback.                   |
| Development Tooling | Testing, formatting, linting and CI automation.                            |

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
pathfinding and road validation to ensure realistic movement behavior.

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
