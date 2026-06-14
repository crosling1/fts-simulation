# FTS Simulation

FTS Simulation is a raylib-based robot logistics simulation project.

The current implementation focuses on the logistics map that will be used before robot behavior is added. The map shows L1-L6 lager positions, pickup lager A, delivery lager B, the robot start position, and road connections to each lager.

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
- `map.cpp`: Map class, lager positions, and road rendering logic

`main.cpp` does not define map details directly. It only calls `InitMap()`, `DrawMap()`, and `UnloadMap()`.
