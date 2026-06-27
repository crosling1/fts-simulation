#pragma once

#include "robots/Battery.h"
#include "robots/RobotRenderData.h"
#include "robots/RobotState.h"
#include "control/PIController.h"
#include "sensors/ProximitySensor.h"
#include "sensors/Sensor.h"
#include "simulation/SimConfig.h"
#include "raylib.h"

#include <memory>
#include <string_view>
#include <vector>

class Robot {
  public:
    using State = RobotState;

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

    Robot(Pose startPose, Config config, SimConfig simConfig = SimConfig::Default());
    Robot(const Vector2& startPosition, Config config, SimConfig simConfig = SimConfig::Default());
    virtual ~Robot() = default;

    [[nodiscard]] virtual std::string_view typeName() const noexcept = 0;

    void updateMovement(float deltaTime);
    [[nodiscard]] RobotRenderData renderData() const noexcept;
    void setPosition(const Vector2& newPosition);
    void setState(State newState);
    void setTargetPosition(const Vector2& target);
    void chargeBy(float amount);
    void enterChargingState();
    void moveTowardsTarget(float deltaTime);
    void rotateTowardsTarget(float deltaTime);
    [[nodiscard]] Vector2 getPosition() const;
    [[nodiscard]] State getState() const;
    [[nodiscard]] bool hasBatteryFull() const;
    [[nodiscard]] bool hasReachedTarget() const;
    [[nodiscard]] float getProximityDetectionRadius() const;
    [[nodiscard]] const Battery& getBattery() const;

    void addSensor(std::unique_ptr<Sensor> sensor);
    void updateSensors();

    [[nodiscard]] double x() const noexcept;
    [[nodiscard]] double y() const noexcept;
    [[nodiscard]] double angle() const noexcept;

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
    SimConfig simConfig_;
    State state_;
    Battery battery_;
    ProximitySensor proximitySensor_;
    std::vector<std::unique_ptr<Sensor>> sensors_;
};
