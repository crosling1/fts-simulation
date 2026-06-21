#include "ui/StatusOverlay.h"

#include "raylib.h"

#include <fstream>
#include <optional>
#include <string>

namespace {
constexpr double memoryRefreshInterval = 0.5;

double lastMemoryRefreshTime = -memoryRefreshInterval;
float cachedMemoryMegabytes = 0.0f;

const char* RobotStateText(Robot::State state) {
    switch (state) {
    case Robot::State::Idle:
        return "Idle";
    case Robot::State::Moving:
        return "Moving";
    case Robot::State::PickingUp:
        return "Picking up";
    case Robot::State::CarryingItem:
        return "Carrying item";
    case Robot::State::DroppingOff:
        return "Dropping off";
    case Robot::State::Arrived:
        return "Arrived";
    case Robot::State::BatteryDepleted:
        return "Battery depleted";
    case Robot::State::Charging:
        return "Charging";
    }

    return "Unknown";
}

float ReadCurrentMemoryMegabytes(void) {
    std::ifstream statusFile("/proc/self/status");
    std::string label;

    while (statusFile >> label) {
        if (label != "VmRSS:") {
            std::string ignoredLine;
            std::getline(statusFile, ignoredLine);
            continue;
        }

        float memoryKilobytes = 0.0f;
        statusFile >> memoryKilobytes;
        return memoryKilobytes / 1024.0f;
    }

    return 0.0f;
}

float GetCurrentMemoryMegabytes(void) {
    const double now = GetTime();
    if (now - lastMemoryRefreshTime >= memoryRefreshInterval) {
        cachedMemoryMegabytes = ReadCurrentMemoryMegabytes();
        lastMemoryRefreshTime = now;
    }

    return cachedMemoryMegabytes;
}
} // namespace

void DrawStatusOverlay(const std::optional<RobotStatusSnapshot>& robotStatus) {
    constexpr int panelWidth = 270;
    constexpr int panelHeight = 112;
    constexpr int panelPadding = 14;
    constexpr int lineHeight = 25;

    const int panelX = GetScreenWidth() - panelWidth - 20;
    constexpr int panelY = 20;

    DrawRectangle(panelX, panelY, panelWidth, panelHeight, Fade(RAYWHITE, 0.92f));
    DrawRectangleLinesEx({(float)panelX, (float)panelY, (float)panelWidth, (float)panelHeight},
                         2.0f, DARKGRAY);

    DrawText("Robot Status", panelX + panelPadding, panelY + 10, 20, DARKGRAY);

    if (!robotStatus.has_value()) {
        DrawText("State: Unavailable", panelX + panelPadding, panelY + 38, 18, RED);
        DrawText("Battery: --", panelX + panelPadding, panelY + 38 + lineHeight, 18, DARKGRAY);
        DrawText(TextFormat("Used Memory: %.1f MB", GetCurrentMemoryMegabytes()),
                 panelX + panelPadding, panelY + 38 + (lineHeight * 2), 18, DARKGRAY);
        return;
    }

    const float batteryPercentage = robotStatus->batteryPercentage;
    const Color batteryColor =
        batteryPercentage <= 10.0f ? RED : batteryPercentage <= 30.0f ? ORANGE : DARKGREEN;

    DrawText(TextFormat("State: %s", RobotStateText(robotStatus->state)), panelX + panelPadding,
             panelY + 38, 18, DARKGRAY);
    DrawText(TextFormat("Battery: %.1f%%", batteryPercentage), panelX + panelPadding,
             panelY + 38 + lineHeight, 18, batteryColor);
    DrawText(TextFormat("Used Memory: %.1f MB", GetCurrentMemoryMegabytes()),
             panelX + panelPadding, panelY + 38 + (lineHeight * 2), 18, DARKGRAY);
}
