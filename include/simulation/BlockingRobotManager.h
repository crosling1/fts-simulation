#pragma once

#include "raylib.h"

#include <cstddef>
#include <random>
#include <vector>

class LogisticsMap;

struct BlockingRobot {
    Vector2 position = {0.0f, 0.0f};
    float radius = 0.0f;
    float speed = 0.0f;
    std::vector<Vector2> path;
    std::size_t currentNodeIndex = 0;
    std::size_t targetNodeIndex = 0;
    std::size_t previousNodeIndex = 0;
    bool active = false;

    void update(float deltaTime, std::mt19937& randomEngine);
    void chooseNextTarget(std::mt19937& randomEngine, bool allowBacktracking);
    [[nodiscard]] bool isNear(Vector2 position, float detectionRadius) const;
    [[nodiscard]] Vector2 getPosition() const noexcept;
    [[nodiscard]] float getRadius() const noexcept;
    [[nodiscard]] bool isActive() const noexcept;
};

class BlockingRobotManager {
  public:
    BlockingRobotManager();

    void clear();
    void addBlockingRobot(const BlockingRobot& blockingRobot);
    void initBlockingRobots(const LogisticsMap& logisticsMap);
    void update(float deltaTime);
    void draw() const;
    [[nodiscard]] bool hasActiveBlockingRobotNear(Vector2 position, float detectionRadius) const;
    [[nodiscard]] const std::vector<BlockingRobot>& getBlockingRobots() const;

  private:
    std::vector<BlockingRobot> blockingRobots;
    std::mt19937 randomEngine;

    void addBlockingRobotPath(const std::vector<Vector2>& path, float speed);
};
