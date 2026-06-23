#include "simulation/BlockingRobotManager.h"

#include "simulation/map.h"

#include <cmath>

namespace {
constexpr float reachedDistance = 2.0f;
constexpr float blockingRobotRadius = 14.0f;
constexpr float blockingRobotSpeed = 65.0f;

float Distance(Vector2 from, Vector2 to) {
    const float deltaX = to.x - from.x;
    const float deltaY = to.y - from.y;

    return std::sqrt((deltaX * deltaX) + (deltaY * deltaY));
}

bool IsBlockingRobotInRange(Vector2 position, const BlockingRobot& blockingRobot,
                            float detectionRadius) {
    return blockingRobot.active &&
           CheckCollisionCircles(position, detectionRadius, blockingRobot.position,
                                 blockingRobot.radius);
}

} // namespace

BlockingRobotManager::BlockingRobotManager(void) : randomEngine(std::random_device{}()) {}

void BlockingRobotManager::clear(void) {
    blockingRobots.clear();
}

void BlockingRobotManager::addBlockingRobot(const BlockingRobot& blockingRobot) {
    blockingRobots.push_back(blockingRobot);
}

void BlockingRobotManager::initBlockingRobots(const LogisticsMap& logisticsMap) {
    clear();

    for (const BlockingRobotPath& blockingPath : logisticsMap.getBlockingRobotPaths()) {
        addBlockingRobotPath(blockingPath.points,
                             blockingRobotSpeed * blockingPath.speedMultiplier);
    }
}

void BlockingRobotManager::update(float deltaTime) {
    for (BlockingRobot& blockingRobot : blockingRobots) {
        if (!blockingRobot.active) {
            continue;
        }

        moveBlockingRobot(blockingRobot, deltaTime);
    }
}

void BlockingRobotManager::draw(void) const {
    for (const BlockingRobot& blockingRobot : blockingRobots) {
        if (!blockingRobot.active) {
            continue;
        }

        DrawCircleV(blockingRobot.position, blockingRobot.radius, PURPLE);
        DrawCircleLines((int)blockingRobot.position.x, (int)blockingRobot.position.y,
                        blockingRobot.radius, DARKPURPLE);
        DrawText("B", (int)blockingRobot.position.x - 5, (int)blockingRobot.position.y - 10, 20,
                 WHITE);
    }
}

bool BlockingRobotManager::hasActiveBlockingRobotNear(Vector2 position,
                                                      float detectionRadius) const {
    for (const BlockingRobot& blockingRobot : blockingRobots) {
        if (IsBlockingRobotInRange(position, blockingRobot, detectionRadius)) {
            return true;
        }
    }

    return false;
}

const std::vector<BlockingRobot>& BlockingRobotManager::getBlockingRobots(void) const {
    return blockingRobots;
}

void BlockingRobotManager::addBlockingRobotPath(const std::vector<Vector2>& path, float speed) {
    if (path.size() < 2) {
        return;
    }

    addBlockingRobot({
        path[0],
        blockingRobotRadius,
        speed,
        path,
        0,
        1,
        0,
        true,
    });
}

void BlockingRobotManager::moveBlockingRobot(BlockingRobot& blockingRobot, float deltaTime) {
    if (blockingRobot.path.empty()) {
        return;
    }

    const Vector2 target = blockingRobot.path[blockingRobot.targetNodeIndex];
    const float distance = Distance(blockingRobot.position, target);
    if (distance <= reachedDistance) {
        blockingRobot.position = target;
        blockingRobot.previousNodeIndex = blockingRobot.currentNodeIndex;
        blockingRobot.currentNodeIndex = blockingRobot.targetNodeIndex;
        chooseNextTarget(blockingRobot, false);
        return;
    }

    const float step = blockingRobot.speed * deltaTime;
    if (step >= distance) {
        blockingRobot.position = target;
        return;
    }

    blockingRobot.position.x += ((target.x - blockingRobot.position.x) / distance) * step;
    blockingRobot.position.y += ((target.y - blockingRobot.position.y) / distance) * step;
}

void BlockingRobotManager::chooseNextTarget(BlockingRobot& blockingRobot, bool allowBacktracking) {
    if (blockingRobot.path.size() < 2) {
        blockingRobot.targetNodeIndex = blockingRobot.currentNodeIndex;
        return;
    }

    std::vector<std::size_t> candidates;
    if (blockingRobot.currentNodeIndex > 0) {
        candidates.push_back(blockingRobot.currentNodeIndex - 1);
    }

    if (blockingRobot.currentNodeIndex + 1 < blockingRobot.path.size()) {
        candidates.push_back(blockingRobot.currentNodeIndex + 1);
    }

    if (!allowBacktracking && candidates.size() > 1) {
        std::vector<std::size_t> forwardCandidates;
        for (std::size_t candidate : candidates) {
            if (candidate != blockingRobot.previousNodeIndex) {
                forwardCandidates.push_back(candidate);
            }
        }

        if (!forwardCandidates.empty()) {
            candidates = forwardCandidates;
        }
    }

    std::uniform_int_distribution<std::size_t> targetDistribution(0, candidates.size() - 1);
    blockingRobot.targetNodeIndex = candidates[targetDistribution(randomEngine)];
}
