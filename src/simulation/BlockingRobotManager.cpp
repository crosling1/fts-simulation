#include "simulation/BlockingRobotManager.h"

#include "simulation/MathUtils.h"
#include "simulation/map.h"

BlockingRobotManager::BlockingRobotManager(const SimConfig& simConfig)
    : simConfig_(simConfig), randomEngine_(std::random_device{}()) {}

BlockingRobot BlockingRobot::AtPosition(Vec2 position, BlockingRobotRadius radius) {
    BlockingRobot blockingRobot;
    blockingRobot.position = position;
    blockingRobot.radius = radius.value;
    return blockingRobot;
}

BlockingRobot BlockingRobot::WithPath(const std::vector<Vec2>& path, BlockingRobotRadius radius,
                                      BlockingRobotSpeed speed) {
    BlockingRobot blockingRobot = AtPosition(path[0], radius);
    blockingRobot.speed = speed.value;
    blockingRobot.path = path;
    blockingRobot.currentNodeIndex = 0;
    blockingRobot.targetNodeIndex = 1;
    blockingRobot.previousNodeIndex = 0;
    return blockingRobot;
}

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

bool BlockingRobotManager::hasActiveBlockingRobotNear(Vec2 position, float detectionRadius) const {
    for (const BlockingRobot& blockingRobot : blockingRobots_) {
        const float combinedRadius = detectionRadius + blockingRobot.radius;
        if (math::distanceSq(position, blockingRobot.position) <= combinedRadius * combinedRadius) {
            return true;
        }
    }

    return false;
}

const std::vector<BlockingRobot>& BlockingRobotManager::getBlockingRobots() const {
    return blockingRobots_;
}

void BlockingRobotManager::addBlockingRobotPath(const std::vector<Vec2>& path, float speed) {
    if (path.size() < 2) {
        return;
    }

    addBlockingRobot(BlockingRobot::WithPath(
        path, BlockingRobotRadius{simConfig_.blockingRobotRadius}, BlockingRobotSpeed{speed}));
}

void BlockingRobotManager::moveBlockingRobot(BlockingRobot& blockingRobot, float deltaTime) {
    if (blockingRobot.path.empty()) {
        return;
    }

    const Vec2 target = blockingRobot.path[blockingRobot.targetNodeIndex];
    const float distance = math::distance(blockingRobot.position, target);
    if (distance <= simConfig_.reachedDistance) {
        blockingRobot.position = target;
        blockingRobot.previousNodeIndex = blockingRobot.currentNodeIndex;
        blockingRobot.currentNodeIndex = blockingRobot.targetNodeIndex;
        chooseNextTarget(blockingRobot, BacktrackPolicy::Prevent);
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

void BlockingRobotManager::chooseNextTarget(BlockingRobot& blockingRobot, BacktrackPolicy policy) {
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

    if (policy == BacktrackPolicy::Prevent && candidates.size() > 1) {
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
