#pragma once

#include "simulation/RobotStatusSnapshot.h"

#include <optional>

void DrawStatusOverlay(const std::optional<RobotStatusSnapshot>& robotStatus);
