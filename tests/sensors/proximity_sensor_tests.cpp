#include "support/test_helpers.h"
#include "support/test_runner.h"
#include "support/test_suites.h"

#include "sensors/ProximitySensor.h"
#include "simulation/BlockingRobotManager.h"

#include <string>

namespace {
void TestProximitySensorUsesBlockingRobotManager(void) {
    BlockingRobotManager manager;
    manager.addBlockingRobot({
        {8.0f, 0.0f},
        2.0f,
        0.0f,
        {},
        0,
        0,
        0,
        true,
    });
    ProximitySensor proximitySensor(6.0f);

    test::Expect(test::AlmostEqual(proximitySensor.getDetectionRadius(), 6.0f),
                 "proximity sensor should expose detection radius");
    test::Expect(proximitySensor.hasBlockingRobotNearby({0.0f, 0.0f}, manager),
                 "proximity sensor should report nearby blocking robots");
}

const test::TestCase proximitySensorTests[] = {
    {"Proximity sensor uses blocking robot manager", TestProximitySensorUsesBlockingRobotManager},
};
} // namespace

void RunProximitySensorTests(void) {
    test::RunTestCases(proximitySensorTests);
}

bool RunProximitySensorTestByName(const std::string& name) {
    return test::RunTestCaseByName(proximitySensorTests, name);
}
