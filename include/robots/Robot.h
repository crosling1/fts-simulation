#ifndef ROBOT_H
#define ROBOT_H

#include "robots/Battery.h"
#include "sensors/Sensor.h"
#include "raylib.h"

#include <cstdint>
#include <memory>
#include <vector>

class Robot {
  public:
    enum class State : std::uint8_t {
        Idle,
        Moving,
        PickingUp,
        CarryingItem,
        DroppingOff,
        Arrived,
        BatteryDepleted,
        Charging,
    };

    struct MotionConfig {
        float speed;
        float rotationSpeed;
        float size;
    };

    struct ControllerConfig {
        float proportionalGain = 0.0f;
        float integralGain = 0.0f;
        float maxIntegralError = 1000.0f;
    };

    struct Config {
        MotionConfig motion;
        ControllerConfig controller;
    };

    Robot(double x, double y, double angle = 0.0);
    Robot(const Vector2& startPosition, Config config);
    virtual ~Robot() = default;

    virtual void printType() const = 0;

    void updateMovement(float deltaTime);
    void draw(void);
    void setPosition(const Vector2& newPosition);
    void setState(State newState);
    void setTargetPosition(const Vector2& target);
    void moveTowardsTarget(float deltaTime);
    void rotateTowardsTarget(float deltaTime);
    void getPosition(Vector2& outPosition) const;
    void getRotation(float& outRotation) const;
    State getState(void) const;
    bool hasReachedTarget(void) const;
    Battery& getBattery(void);
    const Battery& getBattery(void) const;

    void addSensor(std::unique_ptr<Sensor> sensor);
    void updateSensors();

    double x() const;
    double y() const;
    double angle() const;

  protected:
    void moveForward(double distance);
    void rotate(double degree);

  private:
    double x_;
    double y_;
    double angle_;
    float speed_;
    Vector2 targetPosition_;
    float rotationSpeed_;
    float size_;
    float proportionalGain_;
    float integralGain_;
    float maxIntegralError_;
    float distanceErrorIntegral_;
    State state_;
    Battery battery_;
    std::vector<std::unique_ptr<Sensor>> sensors_;
};

#endif // ROBOT_H
