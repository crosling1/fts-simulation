BUILD_DIR := build
CLANG_TIDY ?= clang-tidy
STRICT_CLANG_TIDY_CHECKS ?= 0
FORMAT_FILES := $(shell find include src tests -type f \( -name '*.h' -o -name '*.cpp' \))
TIDY_FILES := $(shell find src tests -type f -name '*.cpp')

.PHONY: configure build run test check ci fix format-check format lint lint-fix clean

configure:
	cmake -S . -B $(BUILD_DIR)

build: configure
	cmake --build $(BUILD_DIR)

run: build
	./$(BUILD_DIR)/robot_sim

test: configure
	cmake --build $(BUILD_DIR) --target unit_tests
	ctest --test-dir $(BUILD_DIR) --output-on-failure

check: test format-check lint

ci: configure build test format-check lint

fix: format lint-fix

format-check: configure
	clang-format --dry-run --Werror $(FORMAT_FILES)

format: configure
	clang-format -i $(FORMAT_FILES)

lint: configure
	cmake --build $(BUILD_DIR)
	CLANG_TIDY=$(CLANG_TIDY) STRICT_CLANG_TIDY_CHECKS=$(STRICT_CLANG_TIDY_CHECKS) bash tools/run-clang-tidy.sh $(TIDY_FILES) -p $(BUILD_DIR) --warnings-as-errors=*

lint-fix: configure
	cmake --build $(BUILD_DIR)
	CLANG_TIDY=$(CLANG_TIDY) STRICT_CLANG_TIDY_CHECKS=$(STRICT_CLANG_TIDY_CHECKS) bash tools/run-clang-tidy.sh $(TIDY_FILES) -p $(BUILD_DIR) -fix --warnings-as-errors=*

clean:
	cmake --build $(BUILD_DIR) --target clean
