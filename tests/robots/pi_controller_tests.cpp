#include "support/test_helpers.h"
#include "support/test_runner.h"
#include "support/test_suites.h"

#include "control/PIController.h"

#include <string>

namespace {
void TestPiControllerUsesMaxOutputWithoutGains(void) {
    PIController controller;

    test::Expect(test::AlmostEqual(controller.update(10.0f, 1.0f, 25.0f), 25.0f),
                 "PI controller without gains should return max output");
}

void TestPiControllerClampsOutput(void) {
    PIController controller({2.0f, 1.0f, 100.0f});

    test::Expect(test::AlmostEqual(controller.update(100.0f, 1.0f, 50.0f), 50.0f),
                 "PI controller output should clamp to max output");
}

void TestPiControllerResetClearsIntegral(void) {
    PIController controller({0.0f, 1.0f, 100.0f});

    test::Expect(test::AlmostEqual(controller.update(10.0f, 1.0f, 100.0f), 10.0f),
                 "PI controller should integrate first error");
    test::Expect(test::AlmostEqual(controller.update(10.0f, 1.0f, 100.0f), 20.0f),
                 "PI controller should accumulate integral error");

    controller.reset();

    test::Expect(test::AlmostEqual(controller.update(10.0f, 1.0f, 100.0f), 10.0f),
                 "PI controller reset should clear integral error");
}

const test::TestCase piControllerTests[] = {
    {"PI controller uses max output without gains", TestPiControllerUsesMaxOutputWithoutGains},
    {"PI controller clamps output", TestPiControllerClampsOutput},
    {"PI controller reset clears integral", TestPiControllerResetClearsIntegral},
};
} // namespace

void RunPiControllerTests(void) {
    test::RunTestCases(piControllerTests);
}

bool RunPiControllerTestByName(const std::string& name) {
    return test::RunTestCaseByName(piControllerTests, name);
}
