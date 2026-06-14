# FTS Simulation

FTS Simulation is a raylib-based robot logistics simulation project.

The current implementation shows a logistics map with L1-L6 lager positions, pickup lager A, delivery lager B, the robot start position, and the road network between lager dock points. A robot starts at the map start position, follows road waypoints to pickup lager A, picks up an item, carries it to delivery lager B, drops it off, and stays constrained to the road network during movement.

## Simulation Behavior

- The map renders warehouses, lager dock points, the robot start position, and the road network.
- The robot controller owns the robot lifecycle and task flow.
- The robot follows waypoint routes instead of moving directly through non-road areas.
- Road constraints clamp the robot back to the nearest road if its center leaves the road network.
- The robot state includes idle, moving, picking up, carrying an item, dropping off, and arrived.
- The carried item is drawn on top of the robot and moves with it while pickup, carry, and dropoff states are active.

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
- `Robot.h`: Robot class interface, movement state, and drawing API
- `Robot.cpp`: Robot movement, state handling, and robot/item rendering
- `RobotController.h`: Public robot controller module functions
- `RobotController.cpp`: Robot task flow, waypoint routing, pickup/dropoff handling, and road enforcement

`main.cpp` does not define map or robot task details directly. It initializes the map and robot controller, updates the controller each frame, and draws the map before drawing the robot controller.
