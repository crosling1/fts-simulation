#ifndef WORKER_ROBOT_H
#define WORKER_ROBOT_H

#include "robots/Robot.h"

class WorkerRobot : public Robot {
  public:
    WorkerRobot(double x, double y, double angle = 0.0);
    WorkerRobot(const Vector2& startPosition, Config config);

    void printType() const override;
};

#endif // WORKER_ROBOT_H
