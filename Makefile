BUILD_DIR := build

.PHONY: configure build run test check fix format-check format lint lint-fix clean

configure:
	cmake -S . -B $(BUILD_DIR)

build: configure
	cmake --build $(BUILD_DIR)

run: build
	./$(BUILD_DIR)/robot_sim

test: configure
	cmake --build $(BUILD_DIR) --target robot_sim

check: configure
	cmake --build $(BUILD_DIR)

fix: configure
	cmake --build $(BUILD_DIR)

format-check: configure
	cmake --build $(BUILD_DIR)

format: configure
	cmake --build $(BUILD_DIR)

lint: configure
	cmake --build $(BUILD_DIR)

lint-fix: configure
	cmake --build $(BUILD_DIR)

clean:
	cmake --build $(BUILD_DIR) --target clean
