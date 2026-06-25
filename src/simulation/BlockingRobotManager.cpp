#include "simulation/BlockingRobotManager.h"
#include "simulation/SimConstants.h"

#include "simulation/map.h"

#include <cmath>

namespace {
float Distance(Vector2 from, Vector2 to) {
    const float deltaX = to.x - from.x;
    const float deltaY = to.y - from.y;

    return std::sqrt((deltaX * deltaX) + (deltaY * deltaY));
}

} // namespace

void BlockingRobot::update(float deltaTime, std::mt19937& randomEngine) {
    if (!active || path.empty()) {
        return;
    }

    const Vector2 target = path[targetNodeIndex];
    const float distance = Distance(position, target);
    if (distance <= SimConstants::kReachedDistance) {
        position = target;
        previousNodeIndex = currentNodeIndex;
        currentNodeIndex = targetNodeIndex;
        chooseNextTarget(randomEngine, false);
        return;
    }

    const float step = speed * deltaTime;
    if (step >= distance) {
        position = target;
        return;
    }

    position.x += ((target.x - position.x) / distance) * step;
    position.y += ((target.y - position.y) / distance) * step;
}

void BlockingRobot::chooseNextTarget(std::mt19937& randomEngine, bool allowBacktracking) {
    if (path.size() < 2) {
        targetNodeIndex = currentNodeIndex;
        return;
    }

    std::vector<std::size_t> candidates;
    if (currentNodeIndex > 0) {
        candidates.push_back(currentNodeIndex - 1);
    }

    if (currentNodeIndex + 1 < path.size()) {
        candidates.push_back(currentNodeIndex + 1);
    }

    if (!allowBacktracking && candidates.size() > 1) {
        std::vector<std::size_t> forwardCandidates;
        for (std::size_t candidate : candidates) {
            if (candidate != previousNodeIndex) {
                forwardCandidates.push_back(candidate);
            }
        }

        if (!forwardCandidates.empty()) {
            candidates = forwardCandidates;
        }
    }

    std::uniform_int_distribution<std::size_t> targetDistribution(0, candidates.size() - 1);
    targetNodeIndex = candidates[targetDistribution(randomEngine)];
}

bool BlockingRobot::isNear(Vector2 otherPosition, float detectionRadius) const {
    return active && CheckCollisionCircles(otherPosition, detectionRadius, position, radius);
}

Vector2 BlockingRobot::getPosition() const noexcept {
    return position;
}

float BlockingRobot::getRadius() const noexcept {
    return radius;
}

bool BlockingRobot::isActive() const noexcept {
    return active;
}

BlockingRobotManager::BlockingRobotManager() : randomEngine(std::random_device{}()) {}

void BlockingRobotManager::clear() {
    blockingRobots.clear();
}

void BlockingRobotManager::addBlockingRobot(const BlockingRobot& blockingRobot) {
    blockingRobots.push_back(blockingRobot);
}

void BlockingRobotManager::initBlockingRobots(const LogisticsMap& logisticsMap) {
    clear();

    for (const BlockingRobotPath& blockingPath : logisticsMap.getBlockingRobotPaths()) {
        addBlockingRobotPath(blockingPath.points,
                             SimConstants::kBlockingRobotSpeed * blockingPath.speedMultiplier);
    }
}

void BlockingRobotManager::update(float deltaTime) {
    for (BlockingRobot& blockingRobot : blockingRobots) {
        blockingRobot.update(deltaTime, randomEngine);
    }
}

void BlockingRobotManager::draw() const {
    for (const BlockingRobot& blockingRobot : blockingRobots) {
        if (!blockingRobot.isActive()) {
            continue;
        }

        const Vector2 position = blockingRobot.getPosition();
        const float radius = blockingRobot.getRadius();

        DrawCircleV(position, radius, PURPLE);
        DrawCircleLines((int)position.x, (int)position.y, radius, DARKPURPLE);
        DrawText("B", (int)position.x - 5, (int)position.y - 10, 20, WHITE);
    }
}

bool BlockingRobotManager::hasActiveBlockingRobotNear(Vector2 position,
                                                      float detectionRadius) const {
    for (const BlockingRobot& blockingRobot : blockingRobots) {
        if (blockingRobot.isNear(position, detectionRadius)) {
            return true;
        }
    }

    return false;
}

const std::vector<BlockingRobot>& BlockingRobotManager::getBlockingRobots() const {
    return blockingRobots;
}

void BlockingRobotManager::addBlockingRobotPath(const std::vector<Vector2>& path, float speed) {
    if (path.size() < 2) {
        return;
    }

    addBlockingRobot({
        path[0],
        SimConstants::kBlockingRobotRadius,
        speed,
        path,
        0,
        1,
        0,
        true,
    });
}
