# FTS Simulation

FTS Simulation is a raylib-based robot logistics simulation project.

The current implementation shows a logistics map with L1-L6 lager positions, pickup lager A, delivery lager B, the robot start position, and the road network between lager dock points. A robot starts at the map start position, requests an A* route through the road network, follows the returned waypoints to pickup lager A, picks up an item, carries it to delivery lager B, drops it off, and stays constrained to the road network during movement. Moving blocking robots share the road network, and the main robot uses a circular lidar scan to wait when another robot blocks its path.

## Simulation Behavior

- The map renders warehouses, lager dock points, the robot start position, and the road network.
- The robot controller owns the robot lifecycle and task flow.
- The navigation module calculates road-network waypoint routes with A* and validates candidate edges against the map road area.
- The robot follows calculated waypoint routes instead of moving directly through non-road areas.
- Road constraints clamp the robot back to the nearest road if its center leaves the road network.
- The robot state includes idle, moving, picking up, carrying an item, dropping off, and arrived.
- The carried item is drawn on top of the robot and moves with it while pickup, carry, and dropoff states are active.
- A lidar sensor draws a circular scan area around the main robot.
- Blocking robots move on road-network paths and choose randomized next targets at path nodes.
- If the main robot detects a blocking robot inside its lidar range, it pauses until the scan area is clear.
- Blocking robots keep moving through blocked narrow sections instead of bouncing off the main robot.

## Requirements

- CMake 3.16+
- C++17 compiler
- raylib
- clang-format
- clang-tidy

On Ubuntu-based systems, the following packages are usually required.

```bash
sudo apt install cmake g++ pkg-config clang-format clang-tidy
```

raylib can be provided by a system package or by a locally built installation.

## Build

```bash
make build
```

## Run

```bash
make run
```

## Format And Lint

Run the full validation pipeline:

```bash
make check
```

Apply automatic fixes:

```bash
make fix
```

Check source formatting:

```bash
make format-check
```

Apply automatic formatting:

```bash
make format
```

Run lint checks:

```bash
make lint
```

Run lint checks with automatic fixes:

```bash
make lint-fix
```

The Makefile wraps CMake, so the equivalent `cmake --build build --target ...` commands still work when needed.

## Project Structure

- `main.cpp`: Program entry point and main loop
- `map.h`: Public map module functions
- `map.cpp`: Map class, lager positions, road rendering logic, and road constraint helpers
- `navigation.h`: Public navigation pathfinding interface
- `navigation.cpp`: Road graph definition, A* waypoint pathfinding, and map-road edge validation
- `Robot.h`: Robot class interface, movement state, and drawing API
- `Robot.cpp`: Robot movement, state handling, and robot/item rendering
- `RobotController.h`: Public robot controller module functions
- `RobotController.cpp`: Robot task flow, navigation requests, pickup/dropoff handling, lidar checks, blocking robot setup, and road enforcement
- `LidarSensor.h`: Public lidar sensor interface
- `LidarSensor.cpp`: Lidar scan checks and scan area rendering
- `ObstacleManager.h`: Blocking robot data and manager interface
- `ObstacleManager.cpp`: Blocking robot movement, randomized target selection, pass-through behavior, and drawing

`main.cpp` does not define map or robot task details directly. It initializes the map and robot controller, updates the controller each frame, and draws the map before drawing the robot controller.
