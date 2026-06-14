#include "ObstacleManager.h"

#include <cmath>

namespace {
constexpr float minBlockingRobotSpeed = 45.0f;
constexpr float maxBlockingRobotSpeed = 95.0f;
constexpr float reachedDistance = 2.0f;

float Distance(Vector2 from, Vector2 to) {
    const float deltaX = to.x - from.x;
    const float deltaY = to.y - from.y;

    return std::sqrt((deltaX * deltaX) + (deltaY * deltaY));
}

bool IsObstacleInRange(Vector2 position, const Obstacle& obstacle, float detectionRadius) {
    return obstacle.active &&
           CheckCollisionCircles(position, detectionRadius, obstacle.position, obstacle.radius);
}

} // namespace

ObstacleManager::ObstacleManager(void) : randomEngine(std::random_device{}()) {}

void ObstacleManager::clear(void) {
    obstacles.clear();
}

void ObstacleManager::addObstacle(const Obstacle& obstacle) {
    obstacles.push_back(obstacle);
}

void ObstacleManager::update(float deltaTime, Vector2 robotPosition, float robotRadius) {
    for (Obstacle& obstacle : obstacles) {
        if (!obstacle.active) {
            continue;
        }

        moveObstacle(obstacle, deltaTime);

        if (CheckCollisionCircles(robotPosition, robotRadius, obstacle.position, obstacle.radius)) {
            bounceObstacle(obstacle);
        }
    }
}

void ObstacleManager::draw(void) const {
    for (const Obstacle& obstacle : obstacles) {
        if (!obstacle.active) {
            continue;
        }

        DrawCircleV(obstacle.position, obstacle.radius, PURPLE);
        DrawCircleLines((int)obstacle.position.x, (int)obstacle.position.y, obstacle.radius,
                        DARKPURPLE);
        DrawText("B", (int)obstacle.position.x - 5, (int)obstacle.position.y - 10, 20, WHITE);
    }
}

bool ObstacleManager::hasActiveObstacleNear(Vector2 position, float detectionRadius) const {
    for (const Obstacle& obstacle : obstacles) {
        if (IsObstacleInRange(position, obstacle, detectionRadius)) {
            return true;
        }
    }

    return false;
}

const std::vector<Obstacle>& ObstacleManager::getObstacles(void) const {
    return obstacles;
}

void ObstacleManager::moveObstacle(Obstacle& obstacle, float deltaTime) {
    if (obstacle.path.empty()) {
        return;
    }

    const Vector2 target = obstacle.path[obstacle.targetNodeIndex];
    const float distance = Distance(obstacle.position, target);
    if (distance <= reachedDistance) {
        obstacle.position = target;
        obstacle.previousNodeIndex = obstacle.currentNodeIndex;
        obstacle.currentNodeIndex = obstacle.targetNodeIndex;
        chooseNextTarget(obstacle, false);
        return;
    }

    const float step = obstacle.speed * deltaTime;
    if (step >= distance) {
        obstacle.position = target;
        return;
    }

    obstacle.position.x += ((target.x - obstacle.position.x) / distance) * step;
    obstacle.position.y += ((target.y - obstacle.position.y) / distance) * step;
}

void ObstacleManager::chooseNextTarget(Obstacle& obstacle, bool allowBacktracking) {
    if (obstacle.path.size() < 2) {
        obstacle.targetNodeIndex = obstacle.currentNodeIndex;
        return;
    }

    std::vector<std::size_t> candidates;
    if (obstacle.currentNodeIndex > 0) {
        candidates.push_back(obstacle.currentNodeIndex - 1);
    }

    if (obstacle.currentNodeIndex + 1 < obstacle.path.size()) {
        candidates.push_back(obstacle.currentNodeIndex + 1);
    }

    if (!allowBacktracking && candidates.size() > 1) {
        std::vector<std::size_t> forwardCandidates;
        for (std::size_t candidate : candidates) {
            if (candidate != obstacle.previousNodeIndex) {
                forwardCandidates.push_back(candidate);
            }
        }

        if (!forwardCandidates.empty()) {
            candidates = forwardCandidates;
        }
    }

    std::uniform_int_distribution<std::size_t> targetDistribution(0, candidates.size() - 1);
    obstacle.targetNodeIndex = candidates[targetDistribution(randomEngine)];
}

void ObstacleManager::bounceObstacle(Obstacle& obstacle) {
    obstacle.targetNodeIndex = obstacle.previousNodeIndex;

    std::uniform_real_distribution<float> speedDistribution(minBlockingRobotSpeed,
                                                            maxBlockingRobotSpeed);
    obstacle.speed = speedDistribution(randomEngine);

    if (obstacle.targetNodeIndex == obstacle.currentNodeIndex) {
        chooseNextTarget(obstacle, true);
    }
}
