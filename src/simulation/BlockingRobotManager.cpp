#include "simulation/BlockingRobotManager.h"

#include "simulation/MathUtils.h"
#include "simulation/map.h"

BlockingRobotManager::BlockingRobotManager(const SimConfig& simConfig)
    : simConfig_(simConfig), randomEngine_(std::random_device{}()) {}

void BlockingRobotManager::clear() {
    blockingRobots_.clear();
}

void BlockingRobotManager::addBlockingRobot(const BlockingRobot& blockingRobot) {
    blockingRobots_.push_back(blockingRobot);
}

void BlockingRobotManager::initBlockingRobots(const LogisticsMap& logisticsMap) {
    clear();

    for (const BlockingRobotPath& blockingPath : logisticsMap.getBlockingRobotPaths()) {
        addBlockingRobotPath(blockingPath.points,
                             simConfig_.blockingRobotSpeed * blockingPath.speedMultiplier);
    }
}

void BlockingRobotManager::update(float deltaTime) {
    for (BlockingRobot& blockingRobot : blockingRobots_) {
        moveBlockingRobot(blockingRobot, deltaTime);
    }
}

void BlockingRobotManager::draw() const {
    for (const BlockingRobot& blockingRobot : blockingRobots_) {
        const Vector2 position = blockingRobot.position;
        const float radius = blockingRobot.radius;

        DrawCircleV(position, radius, PURPLE);
        DrawCircleLines((int)position.x, (int)position.y, radius, DARKPURPLE);
        DrawText("B", (int)position.x - 5, (int)position.y - 10, 20, WHITE);
    }
}

bool BlockingRobotManager::hasActiveBlockingRobotNear(Vector2 position,
                                                      float detectionRadius) const {
    for (const BlockingRobot& blockingRobot : blockingRobots_) {
        if (CheckCollisionCircles(position, detectionRadius, blockingRobot.position,
                                  blockingRobot.radius)) {
            return true;
        }
    }

    return false;
}

const std::vector<BlockingRobot>& BlockingRobotManager::getBlockingRobots() const {
    return blockingRobots_;
}

void BlockingRobotManager::addBlockingRobotPath(const std::vector<Vector2>& path, float speed) {
    if (path.size() < 2) {
        return;
    }

    addBlockingRobot({
        path[0],
        simConfig_.blockingRobotRadius,
        speed,
        path,
        0,
        1,
        0,
    });
}

void BlockingRobotManager::moveBlockingRobot(BlockingRobot& blockingRobot, float deltaTime) {
    if (blockingRobot.path.empty()) {
        return;
    }

    const Vector2 target = blockingRobot.path[blockingRobot.targetNodeIndex];
    const float distance = math::distance(blockingRobot.position, target);
    if (distance <= simConfig_.reachedDistance) {
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

    if (candidates.empty()) {
        blockingRobot.targetNodeIndex = blockingRobot.currentNodeIndex;
        return;
    }

    std::uniform_int_distribution<std::size_t> targetDistribution(0, candidates.size() - 1);
    blockingRobot.targetNodeIndex = candidates[targetDistribution(randomEngine_)];
}
