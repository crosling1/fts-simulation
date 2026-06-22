#include "support/test_helpers.h"
#include "support/test_runner.h"
#include "support/test_suites.h"

#include "simulation/BlockingRobotManager.h"

#include <string>

namespace {
void TestBlockingRobotDetectionIgnoresInactiveRobots(void) {
    BlockingRobotManager manager;

    manager.addBlockingRobot({
        {10.0f, 0.0f},
        5.0f,
        0.0f,
        {},
        0,
        0,
        0,
        true,
    });
    manager.addBlockingRobot({
        {2.0f, 0.0f},
        5.0f,
        0.0f,
        {},
        0,
        0,
        0,
        false,
    });

    test::Expect(manager.hasActiveBlockingRobotNear({0.0f, 0.0f}, 6.0f),
                 "active blocking robot should be detected when circles overlap");
    test::Expect(!manager.hasActiveBlockingRobotNear({0.0f, 0.0f}, 4.0f),
                 "active blocking robot should not be detected outside combined radii");
}

void TestBlockingRobotOutsideRadiusIsNotDetected(void) {
    BlockingRobotManager manager;
    manager.addBlockingRobot({
        {20.0f, 0.0f},
        2.0f,
        0.0f,
        {},
        0,
        0,
        0,
        true,
    });

    test::Expect(!manager.hasActiveBlockingRobotNear({0.0f, 0.0f}, 6.0f),
                 "blocking robot outside detection radius should not be detected");
}

void TestBlockingRobotInsideRadiusIsDetected(void) {
    BlockingRobotManager manager;
    manager.addBlockingRobot({
        {5.0f, 0.0f},
        2.0f,
        0.0f,
        {},
        0,
        0,
        0,
        true,
    });

    test::Expect(manager.hasActiveBlockingRobotNear({0.0f, 0.0f}, 6.0f),
                 "blocking robot inside detection radius should be detected");
}

void TestBlockingRobotMovesAlongPath(void) {
    BlockingRobotManager manager;
    manager.addBlockingRobot({
        {0.0f, 0.0f},
        5.0f,
        10.0f,
        {{0.0f, 0.0f}, {10.0f, 0.0f}},
        0,
        1,
        0,
        true,
    });

    manager.update(0.5f);
    test::ExpectVectorNear(manager.getBlockingRobots()[0].position, {5.0f, 0.0f},
                           "blocking robot should move along path");

    manager.update(0.5f);
    test::ExpectVectorNear(manager.getBlockingRobots()[0].position, {10.0f, 0.0f},
                           "blocking robot should snap to target when step reaches it");
}

const test::TestCase blockingRobotManagerTests[] = {
    {"Blocking robot detection ignores inactive robots",
     TestBlockingRobotDetectionIgnoresInactiveRobots},
    {"Blocking robot outside radius is not detected", TestBlockingRobotOutsideRadiusIsNotDetected},
    {"Blocking robot inside radius is detected", TestBlockingRobotInsideRadiusIsDetected},
    {"Blocking robot moves along path", TestBlockingRobotMovesAlongPath},
};
} // namespace

void RunBlockingRobotManagerTests(void) {
    test::RunTestCases(blockingRobotManagerTests);
}

bool RunBlockingRobotManagerTestByName(const std::string& name) {
    return test::RunTestCaseByName(blockingRobotManagerTests, name);
}
