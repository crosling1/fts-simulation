#include "support/test_helpers.h"
#include "support/test_runner.h"
#include "support/test_suites.h"

#include "sensors/LidarSensor.h"
#include "simulation/ObstacleManager.h"

#include <string>

namespace {
void TestLidarUsesObstacleManager(void) {
    ObstacleManager manager;
    manager.addObstacle({
        {8.0f, 0.0f},
        2.0f,
        0.0f,
        {},
        0,
        0,
        0,
        true,
    });
    LidarSensor lidar(6.0f);

    test::Expect(test::AlmostEqual(lidar.getDetectionRadius(), 6.0f),
                 "lidar should expose detection radius");
    test::Expect(lidar.hasObstacleNearby({0.0f, 0.0f}, manager),
                 "lidar should report nearby active obstacles");
}

const test::TestCase lidarTests[] = {
    {"Lidar uses obstacle manager", TestLidarUsesObstacleManager},
};
} // namespace

void RunLidarTests(void) {
    test::RunTestCases(lidarTests);
}

bool RunLidarTestByName(const std::string& name) {
    return test::RunTestCaseByName(lidarTests, name);
}
