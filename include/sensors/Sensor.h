#pragma once

class Robot;

class Sensor {
  public:
    virtual ~Sensor() = default;

    virtual void update(const Robot& robot) = 0;
    virtual double value() const = 0;
};
