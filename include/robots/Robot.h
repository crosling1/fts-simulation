#ifndef ROBOT_H
#define ROBOT_H

#include "robots/Battery.h"
#include "control/PIController.h"
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

    using ControllerConfig = PIController::Config;

    struct Config {
        MotionConfig motion;
        ControllerConfig controller;
    };

    struct Pose {
        Vector2 position;
        float angleDegrees = 0.0f;
    };

    Robot(Pose startPose, Config config);
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
    Vector2 getPosition() const;
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
    PIController speedController_;
    State state_;
    Battery battery_;
    std::vector<std::unique_ptr<Sensor>> sensors_;
};

#endif // ROBOT_H
