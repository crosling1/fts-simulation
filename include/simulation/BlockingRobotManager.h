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
    void moveBlockingRobot(BlockingRobot& blockingRobot, float deltaTime);
    void chooseNextTarget(BlockingRobot& blockingRobot, bool allowBacktracking);
};
