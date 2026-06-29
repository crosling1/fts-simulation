BUILD_DIR := build
CMAKE_CACHE := $(BUILD_DIR)/CMakeCache.txt
CLANG_TIDY ?= $(shell command -v clang-tidy-18 >/dev/null 2>&1 && echo clang-tidy-18 || echo clang-tidy)
STRICT_CLANG_TIDY_CHECKS ?= 0
FORMAT_FILES := $(shell find include src tests -type f \( -name '*.h' -o -name '*.cpp' \))
TIDY_FILES := $(shell find src tests -type f -name '*.cpp')

.PHONY: configure build run test check ci fix format-check format lint lint-fix clean

configure: $(CMAKE_CACHE)

$(CMAKE_CACHE):
	cmake -S . -B $(BUILD_DIR)

build: $(CMAKE_CACHE)
	cmake --build $(BUILD_DIR)

run: build
	./$(BUILD_DIR)/robot_sim

test: $(CMAKE_CACHE)
	cmake --build $(BUILD_DIR) --target unit_tests
	ctest --test-dir $(BUILD_DIR) --output-on-failure

check: test format-check lint

ci: configure build test format-check lint

fix: format lint-fix

format-check: $(CMAKE_CACHE)
	clang-format --dry-run --Werror $(FORMAT_FILES)

format: $(CMAKE_CACHE)
	clang-format -i $(FORMAT_FILES)

lint: $(CMAKE_CACHE)
	cmake --build $(BUILD_DIR)
	CLANG_TIDY=$(CLANG_TIDY) STRICT_CLANG_TIDY_CHECKS=$(STRICT_CLANG_TIDY_CHECKS) bash tools/run-clang-tidy.sh $(TIDY_FILES) -p $(BUILD_DIR) --warnings-as-errors=*

lint-fix: $(CMAKE_CACHE)
	cmake --build $(BUILD_DIR)
	CLANG_TIDY=$(CLANG_TIDY) STRICT_CLANG_TIDY_CHECKS=$(STRICT_CLANG_TIDY_CHECKS) bash tools/run-clang-tidy.sh $(TIDY_FILES) -p $(BUILD_DIR) -fix --warnings-as-errors=*

clean:
	cmake --build $(BUILD_DIR) --target clean
