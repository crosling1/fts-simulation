#pragma once

#include "robots/IRobotRenderer.h"

class BlockingRobotManager;
class LogisticsMap;
class RobotController;
class RouteFollower;

void DrawLogisticsMap(const LogisticsMap& logisticsMap);
void DrawBlockingRobots(const BlockingRobotManager& blockingRobotManager);
void DrawRouteFollower(const RouteFollower& routeFollower);
void DrawRobotController(const RobotController& robotController,
                         const IRobotRenderer& robotRenderer);
