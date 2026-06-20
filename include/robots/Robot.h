#ifndef ROBOT_H
#define ROBOT_H

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
    };

    struct Config {
        float speed;
        float rotationSpeed;
        float size;
    };

    Robot(double x, double y, double angle = 0.0);
    Robot(const Vector2& startPosition, Config config);
    virtual ~Robot() = default;

    virtual void update() = 0;
    virtual void printType() const = 0;

    void update(float deltaTime);
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
    State state_;
    std::vector<std::unique_ptr<Sensor>> sensors_;
};

#endif // ROBOT_H
