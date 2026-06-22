#ifndef BLOCKING_ROBOT_MANAGER_H
#define BLOCKING_ROBOT_MANAGER_H

#include "raylib.h"

#include <cstddef>
#include <random>
#include <vector>

struct BlockingRobot {
    Vector2 position;
    float radius;
    float speed;
    std::vector<Vector2> path;
    std::size_t currentNodeIndex;
    std::size_t targetNodeIndex;
    std::size_t previousNodeIndex;
    bool active;
};

class BlockingRobotManager {
  public:
    BlockingRobotManager(void);

    void clear(void);
    void addBlockingRobot(const BlockingRobot& blockingRobot);
    void initBlockingRobots(void);
    void update(float deltaTime);
    void draw(void) const;
    bool hasActiveBlockingRobotNear(Vector2 position, float detectionRadius) const;
    const std::vector<BlockingRobot>& getBlockingRobots(void) const;

  private:
    std::vector<BlockingRobot> blockingRobots;
    std::mt19937 randomEngine;

    void addBlockingRobotPath(const std::vector<Vector2>& path, float speed);
    void moveBlockingRobot(BlockingRobot& blockingRobot, float deltaTime);
    void chooseNextTarget(BlockingRobot& blockingRobot, bool allowBacktracking);
};

#endif // BLOCKING_ROBOT_MANAGER_H
