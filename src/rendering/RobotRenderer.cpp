#include "rendering/RobotRenderer.h"

#include "rendering/RaylibGeometry.h"
#include "simulation/MathUtils.h"

#include <cmath>

namespace {
Color GetRobotColor(RobotState state) {
    switch (state) {
    case RobotState::Idle:
        return GRAY;
    case RobotState::Moving:
        return GREEN;
    case RobotState::PickingUp:
        return YELLOW;
    case RobotState::CarryingItem:
        return ORANGE;
    case RobotState::DroppingOff:
        return SKYBLUE;
    case RobotState::Arrived:
        return LIME;
    case RobotState::BatteryDepleted:
        return RED;
    case RobotState::Charging:
        return BLUE;
    }

    return ORANGE;
}
} // namespace

void RobotRenderer::draw(const RobotRenderData& data) const {
    const Vector2 position = ToRaylib(data.position);

    DrawCircleLines(static_cast<int>(data.position.x), static_cast<int>(data.position.y),
                    data.proximityDetectionRadius, Fade(BLUE, 0.55f));

    const float headingLength = data.radius * 1.15f;
    const float headingRadians = data.angleDegrees / math::kRadToDeg;
    const Vector2 headingEnd = {
        data.position.x + std::cos(headingRadians) * headingLength,
        data.position.y + std::sin(headingRadians) * headingLength,
    };

    DrawCircleV(position, data.radius, GetRobotColor(data.state));
    DrawCircleLines(static_cast<int>(data.position.x), static_cast<int>(data.position.y),
                    data.radius, BLACK);
    DrawCircleLines(static_cast<int>(data.position.x), static_cast<int>(data.position.y),
                    data.radius + 2.0f, WHITE);
    DrawLineEx(position, headingEnd, 3.0f, BLACK);
    DrawCircleV(position, data.radius * 0.2f, BLACK);
    DrawText("R", static_cast<int>(data.position.x) - 5, static_cast<int>(data.position.y) - 10, 20,
             WHITE);

    if (data.carryingItem) {
        const Rect item = {data.position.x - 7.0f, data.position.y - 25.0f, 14.0f, 12.0f};

        DrawRectangleRec(ToRaylib(item), GOLD);
        DrawRectangleLinesEx(ToRaylib(item), 2.0f, BROWN);
    }
}
