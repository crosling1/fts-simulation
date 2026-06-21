#include "support/test_helpers.h"
#include "support/test_runner.h"
#include "support/test_suites.h"

#include "simulation/ObstacleManager.h"

#include <string>

namespace {
void TestObstacleDetectionIgnoresInactiveObstacles(void) {
    ObstacleManager manager;

    manager.addObstacle({
        {10.0f, 0.0f},
        5.0f,
        0.0f,
        {},
        0,
        0,
        0,
        true,
    });
    manager.addObstacle({
        {2.0f, 0.0f},
        5.0f,
        0.0f,
        {},
        0,
        0,
        0,
        false,
    });

    test::Expect(manager.hasActiveObstacleNear({0.0f, 0.0f}, 6.0f),
                 "active obstacle should be detected when circles overlap");
    test::Expect(!manager.hasActiveObstacleNear({0.0f, 0.0f}, 4.0f),
                 "active obstacle should not be detected outside combined radii");
}

void TestObstacleMovesAlongPath(void) {
    ObstacleManager manager;
    manager.addObstacle({
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
    test::ExpectVectorNear(manager.getObstacles()[0].position, {5.0f, 0.0f},
                           "obstacle should move along path");

    manager.update(0.5f);
    test::ExpectVectorNear(manager.getObstacles()[0].position, {10.0f, 0.0f},
                           "obstacle should snap to target when step reaches it");
}

const test::TestCase obstacleTests[] = {
    {"Obstacle detection ignores inactive obstacles",
     TestObstacleDetectionIgnoresInactiveObstacles},
    {"Obstacle moves along path", TestObstacleMovesAlongPath},
};
} // namespace

void RunObstacleTests(void) {
    test::RunTestCases(obstacleTests);
}

bool RunObstacleTestByName(const std::string& name) {
    return test::RunTestCaseByName(obstacleTests, name);
}
