#ifndef ROBOT_H
#define ROBOT_H

#include "raylib.h"

#ifdef __cplusplus
extern "C" {
#endif

class Robot {
  public:
    enum class State {
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

    Robot(const Vector2& startPosition, Config config);

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

  private:
    float speed;
    Vector2 position;
    Vector2 targetPosition;
    float rotation;
    float rotationSpeed;
    float size;
    State state;
};

#ifdef __cplusplus
}
#endif

#endif // ROBOT_H
