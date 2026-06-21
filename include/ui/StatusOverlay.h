#ifndef UI_STATUS_OVERLAY_H
#define UI_STATUS_OVERLAY_H

#include "simulation/RobotStatusSnapshot.h"

#include <optional>

void DrawStatusOverlay(const std::optional<RobotStatusSnapshot>& robotStatus);

#endif // UI_STATUS_OVERLAY_H
