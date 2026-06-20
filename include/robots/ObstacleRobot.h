#ifndef OBSTACLE_ROBOT_H
#define OBSTACLE_ROBOT_H

#include "robots/Robot.h"

class ObstacleRobot : public Robot {
  public:
    using Robot::update;

    ObstacleRobot(double x, double y, double angle = 0.0);

    void update() override;
    void printType() const override;
};

#endif // OBSTACLE_ROBOT_H
