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

float ReadCurrentMemoryMegabytes() {
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

float GetCurrentMemoryMegabytes() {
    const double now = GetTime();
    if (now - lastMemoryRefreshTime >= memoryRefreshInterval) {
        cachedMemoryMegabytes = ReadCurrentMemoryMegabytes();
        lastMemoryRefreshTime = now;
    }

    return cachedMemoryMegabytes;
}

void DrawControlHint() {
    constexpr int fontSize = 20;
    constexpr int horizontalPadding = 18;
    constexpr int hintHeight = 36;
    constexpr int bottomMargin = 18;

    const char* emergencyKey = "E";
    const char* emergencyText = ": Emergency Stop    ";
    const char* resetKey = "R";
    const char* resetText = ": Reset Stop";

    const int textWidth = MeasureText(emergencyKey, fontSize) +
                          MeasureText(emergencyText, fontSize) + MeasureText(resetKey, fontSize) +
                          MeasureText(resetText, fontSize);
    const int hintWidth = textWidth + (horizontalPadding * 2);
    const int hintX = (GetScreenWidth() - hintWidth) / 2;
    const int hintY = GetScreenHeight() - hintHeight - bottomMargin;
    int textX = hintX + horizontalPadding;
    const int textY = hintY + 8;

    DrawRectangle(hintX, hintY, hintWidth, hintHeight, Fade(RAYWHITE, 0.9f));
    DrawRectangleLinesEx({static_cast<float>(hintX), static_cast<float>(hintY),
                          static_cast<float>(hintWidth), static_cast<float>(hintHeight)},
                         2.0f, DARKGRAY);

    DrawText(emergencyKey, textX, textY, fontSize, MAROON);
    textX += MeasureText(emergencyKey, fontSize);
    DrawText(emergencyText, textX, textY, fontSize, DARKGRAY);
    textX += MeasureText(emergencyText, fontSize);
    DrawText(resetKey, textX, textY, fontSize, DARKBLUE);
    textX += MeasureText(resetKey, fontSize);
    DrawText(resetText, textX, textY, fontSize, DARKGRAY);
}
} // namespace

void DrawStatusOverlay(const std::optional<RobotStatusSnapshot>& robotStatus,
                       const SimConfig& simConfig) {
    constexpr int panelWidth = 270;
    constexpr int panelHeight = 138;
    constexpr int panelPadding = 14;
    constexpr int lineHeight = 25;

    const int panelX = GetScreenWidth() - panelWidth - 20;
    constexpr int panelY = 20;

    DrawRectangle(panelX, panelY, panelWidth, panelHeight, Fade(RAYWHITE, 0.92f));
    DrawRectangleLinesEx({static_cast<float>(panelX), static_cast<float>(panelY),
                          static_cast<float>(panelWidth), static_cast<float>(panelHeight)},
                         2.0f, DARKGRAY);

    DrawText("Robot Status", panelX + panelPadding, panelY + 10, 20, DARKGRAY);

    if (!robotStatus.has_value()) {
        DrawText("State: Unavailable", panelX + panelPadding, panelY + 38, 18, RED);
        DrawText("Battery: --", panelX + panelPadding, panelY + 38 + lineHeight, 18, DARKGRAY);
        DrawText(TextFormat("Used Memory: %.1f MB", GetCurrentMemoryMegabytes()),
                 panelX + panelPadding, panelY + 38 + (lineHeight * 2), 18, DARKGRAY);
        DrawText("E-Stop: Unavailable", panelX + panelPadding, panelY + 38 + (lineHeight * 3), 18,
                 RED);
        DrawControlHint();
        return;
    }

    const float batteryPercentage = robotStatus->batteryPercentage;
    const Color batteryColor = batteryPercentage <= simConfig.lowBatteryThreshold       ? RED
                               : batteryPercentage <= simConfig.batteryWarningThreshold ? ORANGE
                                                                                        : DARKGREEN;
    const Color emergencyStopColor = robotStatus->emergencyStopActive ? RED : DARKGREEN;
    const char* emergencyStopText = robotStatus->emergencyStopActive ? "ACTIVE" : "Ready";

    DrawText(TextFormat("State: %s", RobotStateText(robotStatus->state)), panelX + panelPadding,
             panelY + 38, 18, DARKGRAY);
    DrawText(TextFormat("Battery: %.1f%%", batteryPercentage), panelX + panelPadding,
             panelY + 38 + lineHeight, 18, batteryColor);
    DrawText(TextFormat("Used Memory: %.1f MB", GetCurrentMemoryMegabytes()), panelX + panelPadding,
             panelY + 38 + (lineHeight * 2), 18, DARKGRAY);
    DrawText(TextFormat("E-Stop: %s", emergencyStopText), panelX + panelPadding,
             panelY + 38 + (lineHeight * 3), 18, emergencyStopColor);
    DrawControlHint();

    if (robotStatus->emergencyStopActive) {
        DrawRectangle(20, 20, 316, 42, Fade(RED, 0.88f));
        DrawRectangleLinesEx({20.0f, 20.0f, 316.0f, 42.0f}, 2.0f, MAROON);
        DrawText("EMERGENCY STOP ACTIVE", 34, 31, 20, RAYWHITE);
    }
}
