BUILD_DIR := build

.PHONY: configure build run check fix format-check format lint lint-fix clean

configure:
	cmake -S . -B $(BUILD_DIR)

build: configure
	cmake --build $(BUILD_DIR)

run: build
	./$(BUILD_DIR)/robot_sim

check: configure
	cmake --build $(BUILD_DIR)
	cmake --build $(BUILD_DIR) --target format-check
	cmake --build $(BUILD_DIR) --target lint

fix: configure
	cmake --build $(BUILD_DIR) --target format
	cmake --build $(BUILD_DIR) --target lint-fix

format-check: configure
	cmake --build $(BUILD_DIR) --target format-check

format: configure
	cmake --build $(BUILD_DIR) --target format

lint: configure
	cmake --build $(BUILD_DIR) --target lint

lint-fix: configure
	cmake --build $(BUILD_DIR) --target lint-fix

clean:
	cmake --build $(BUILD_DIR) --target clean
