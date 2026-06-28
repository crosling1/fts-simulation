#pragma once

#include "simulation/RobotStatusSnapshot.h"
#include "simulation/SimConfig.h"

#include <optional>

void DrawStatusOverlay(const std::optional<RobotStatusSnapshot>& robotStatus,
                       SimConfig simConfig = SimConfig::Default());
