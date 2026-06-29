#pragma once

#include "simulation/SimConfig.h"

#include "raylib.h"

#include <cstddef>
#include <cstdint>
#include <random>
#include <vector>

class LogisticsMap;

struct BlockingRobotRadius {
    float value;
};

struct BlockingRobotSpeed {
    float value;
};

struct BlockingRobot {
    [[nodiscard]] static BlockingRobot AtPosition(Vector2 position, BlockingRobotRadius radius);
    [[nodiscard]] static BlockingRobot WithPath(const std::vector<Vector2>& path,
                                                BlockingRobotRadius radius,
                                                BlockingRobotSpeed speed);

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
    explicit BlockingRobotManager(const SimConfig& simConfig = SimConfig::Default());

    void clear();
    void addBlockingRobot(const BlockingRobot& blockingRobot);
    void initBlockingRobots(const LogisticsMap& logisticsMap);
    void update(float deltaTime);
    void draw() const;
    [[nodiscard]] bool hasActiveBlockingRobotNear(Vector2 position, float detectionRadius) const;
    [[nodiscard]] const std::vector<BlockingRobot>& getBlockingRobots() const;

  private:
    enum class BacktrackPolicy : std::uint8_t { Allow, Prevent };

    std::vector<BlockingRobot> blockingRobots_;
    SimConfig simConfig_;
    std::mt19937 randomEngine_;

    void addBlockingRobotPath(const std::vector<Vector2>& path, float speed);
    void moveBlockingRobot(BlockingRobot& blockingRobot, float deltaTime);
    void chooseNextTarget(BlockingRobot& blockingRobot, BacktrackPolicy policy);
};
