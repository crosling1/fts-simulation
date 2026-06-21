#ifndef TEST_RUNNER_H
#define TEST_RUNNER_H

#include <cstddef>
#include <iostream>
#include <string>

namespace test {
struct TestCase {
    const char* name;
    void (*run)(void);
};

inline void RunTest(const TestCase& testCase) {
    testCase.run();
    std::cout << "[PASS] " << testCase.name << '\n';
}

template <std::size_t Count> void RunTestCases(const TestCase (&testCases)[Count]) {
    for (const TestCase& testCase : testCases) {
        RunTest(testCase);
    }
}

template <std::size_t Count>
bool RunTestCaseByName(const TestCase (&testCases)[Count], const std::string& name) {
    for (const TestCase& testCase : testCases) {
        if (name == testCase.name) {
            RunTest(testCase);
            return true;
        }
    }

    return false;
}
} // namespace test

#endif // TEST_RUNNER_H
