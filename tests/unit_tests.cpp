#include "support/test_suites.h"

#include <exception>
#include <iostream>
#include <string>

bool RunTestByName(const std::string& name) {
    return RunRobotTestByName(name) || RunBlockingRobotManagerTestByName(name) ||
           RunProximitySensorTestByName(name) || RunMapTestByName(name) ||
           RunNavigationTestByName(name);
}

int main(int argc, char** argv) {
    try {
        if (argc == 3 && std::string(argv[1]) == "--test") {
            if (!RunTestByName(argv[2])) {
                std::cerr << "[FAIL] Unknown test: " << argv[2] << '\n';
                return 1;
            }
            return 0;
        }

        RunRobotTests();
        RunBlockingRobotManagerTests();
        RunProximitySensorTests();
        RunMapTests();
        RunNavigationTests();
    } catch (const std::exception& error) {
        std::cerr << "[FAIL] " << error.what() << '\n';
        return 1;
    }

    return 0;
}
