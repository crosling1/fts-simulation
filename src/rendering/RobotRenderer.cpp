#include "rendering/RobotRenderer.h"

#include <cmath>

namespace {
constexpr float radToDeg = 57.29577951308232f;

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
    DrawCircleLines((int)data.position.x, (int)data.position.y, data.proximityDetectionRadius,
                    Fade(BLUE, 0.55f));

    const float headingLength = data.radius * 1.15f;
    const float headingRadians = data.angleDegrees / radToDeg;
    const Vector2 headingEnd = {
        data.position.x + std::cos(headingRadians) * headingLength,
        data.position.y + std::sin(headingRadians) * headingLength,
    };

    DrawCircleV(data.position, data.radius, GetRobotColor(data.state));
    DrawCircleLines((int)data.position.x, (int)data.position.y, data.radius, BLACK);
    DrawCircleLines((int)data.position.x, (int)data.position.y, data.radius + 2.0f, WHITE);
    DrawLineEx(data.position, headingEnd, 3.0f, BLACK);
    DrawCircleV(data.position, data.radius * 0.2f, BLACK);
    DrawText("R", (int)data.position.x - 5, (int)data.position.y - 10, 20, WHITE);

    if (data.carryingItem) {
        const Rectangle item = {data.position.x - 7.0f, data.position.y - 25.0f, 14.0f, 12.0f};

        DrawRectangleRec(item, GOLD);
        DrawRectangleLinesEx(item, 2.0f, BROWN);
    }
}
