#ifndef OBSTACLE_MANAGER_H
#define OBSTACLE_MANAGER_H

#include "raylib.h"

#include <cstddef>
#include <random>
#include <vector>

struct Obstacle {
    Vector2 position;
    float radius;
    float speed;
    std::vector<Vector2> path;
    std::size_t currentNodeIndex;
    std::size_t targetNodeIndex;
    std::size_t previousNodeIndex;
    bool active;
};

class ObstacleManager {
  public:
    ObstacleManager(void);

    void clear(void);
    void addObstacle(const Obstacle& obstacle);
    void update(float deltaTime, Vector2 robotPosition, float robotRadius);
    void draw(void) const;
    bool hasActiveObstacleNear(Vector2 position, float detectionRadius) const;
    const std::vector<Obstacle>& getObstacles(void) const;

  private:
    std::vector<Obstacle> obstacles;
    std::mt19937 randomEngine;

    void moveObstacle(Obstacle& obstacle, float deltaTime);
    void chooseNextTarget(Obstacle& obstacle, bool allowBacktracking);
    void bounceObstacle(Obstacle& obstacle);
};

#endif // OBSTACLE_MANAGER_H
