# FTS Simulation

FTS Simulation is a raylib-based robot logistics simulation project.

The current implementation shows a logistics map with L1-L6 lager positions, pickup lager A, delivery lager B, a charging station, the robot start position, and the road network between dock points. A robot starts at the map start position, requests A* routes through the road network, repeats pickup and delivery tasks, tracks battery usage while moving, charges when the next delivery would leave too little battery, and stays constrained to the road network during movement. Moving blocking robots share the road network, and the main robot uses a circular lidar scan to wait when another robot blocks its path. The main robot movement uses a simple PI controller and can be stopped with the emergency stop control.

## Simulation Behavior

- The map renders warehouses, lager dock points, the charging station, the robot start position, and the road network.
- The robot controller owns the robot lifecycle and task flow.
- The navigation module calculates road-network waypoint routes with A* and validates candidate edges against the map road area.
- The robot follows calculated waypoint routes instead of moving directly through non-road areas.
- Navigation removes unnecessary pass-through waypoints while keeping turn points needed for dock entry.
- Road constraints clamp the robot back to the nearest road if its center leaves the road network.
- The robot state includes idle, moving, picking up, carrying an item, dropping off, arrived, battery depleted, and charging.
- The main robot movement speed is adjusted by a simple PI controller while still respecting the configured maximum speed.
- The carried item is drawn on top of the robot and moves with it while pickup, carry, and dropoff states are active.
- The robot drains battery based on distance traveled.
- After each dropoff, the controller estimates whether the robot can complete the next pickup and delivery while staying above 10% battery.
- If the next delivery would leave too little battery, or if the battery is 10% or lower after dropoff, the robot goes to the charging station.
- Charging restores battery at 10% per second and then the robot resumes the pickup/delivery loop.
- A status overlay shows the robot state, battery percentage, and currently used process memory.
- The status overlay shows emergency stop state, and the bottom-center control hint shows `E` for emergency stop and `R` for reset.
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
- GitHub Actions is used for CI on pushes and pull requests

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

Run unit tests only:

```bash
make test
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

`make lint` runs `clang-tidy` on project and test `.cpp` files using `build/compile_commands.json`, with warnings treated as errors. The wrapper may print a suppressed-warning summary; those suppressed warnings come from non-user code or lines explicitly marked with `NOLINT`.

Run lint checks with automatic fixes:

```bash
make lint-fix
```

The Makefile wraps CMake, so the equivalent `cmake --build build --target ...` commands still work when needed. Unit tests are registered individually with CTest and can also be run directly with:

```bash
ctest --test-dir build --output-on-failure
./build/unit_tests
./build/unit_tests --test "Robot moves to target"
```

## Continuous Integration

GitHub Actions runs the project validation pipeline for pull requests and pushes to `main` or `master`.

The CI workflow installs the required build tools, builds raylib, configures the project, builds the simulation, runs unit tests, checks formatting, and runs clang-tidy.

## Project Structure

- `main.cpp`: Program entry point and main loop
- `map.h`: Public map module functions
- `map.cpp`: Map class, lager positions, road rendering logic, and road constraint helpers
- `navigation.h`: Public navigation pathfinding interface
- `navigation.cpp`: Road graph definition, A* waypoint pathfinding, and map-road edge validation
- `Robot.h`: Robot class interface, movement state, and drawing API
- `Robot.cpp`: Robot movement, battery drain, state handling, and robot/item rendering
- `Battery.h`: Public battery interface
- `Battery.cpp`: Battery charge, drain, and percentage clamping logic
- `RobotController.h`: Public robot controller module functions
- `RobotController.cpp`: Robot task flow, navigation requests, pickup/dropoff handling, charging decisions, lidar checks, and road enforcement
- `RobotStatusSnapshot.h`: Display-safe robot status data for UI overlays
- `StatusOverlay.h`: Public status overlay drawing interface
- `StatusOverlay.cpp`: Robot status, emergency stop state, control hints, battery, and used-memory overlay rendering
- `LidarSensor.h`: Public lidar sensor interface
- `LidarSensor.cpp`: Lidar scan checks and scan area rendering
- `ObstacleManager.h`: Blocking robot data and manager interface
- `ObstacleManager.cpp`: Blocking robot movement, randomized target selection, pass-through behavior, and drawing
- `tests/unit_tests.cpp`: Unit test executable entry point
- `tests/support/`: Shared unit test helpers, test case runner, and suite declarations
- `tests/robots/`: Robot and battery behavior tests
- `tests/sensors/`: Lidar sensor tests
- `tests/simulation/`: Map, obstacle, and navigation tests

`main.cpp` does not define map, robot task, or UI details directly. It initializes the map and controllers, updates them each frame, and draws the map, blocking robots, main robot, and status overlay in order.
