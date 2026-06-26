#pragma once

#include "raylib.h"

#include <cstddef>
#include <random>
#include <vector>

class LogisticsMap;

struct BlockingRobot {
    Vector2 position{};
    float radius{};
    float speed{};
    std::vector<Vector2> path;
    std::size_t currentNodeIndex{};
    std::size_t targetNodeIndex{};
    std::size_t previousNodeIndex{};
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
    std::vector<BlockingRobot> blockingRobots_;
    std::mt19937 randomEngine_;

    void addBlockingRobotPath(const std::vector<Vector2>& path, float speed);
    void moveBlockingRobot(BlockingRobot& blockingRobot, float deltaTime);
    void chooseNextTarget(BlockingRobot& blockingRobot, bool allowBacktracking);
};
