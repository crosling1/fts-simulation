#include "rendering/SimulationRenderer.h"

#include "rendering/RaylibGeometry.h"
#include "simulation/RouteFollower.h"

void DrawRouteFollower(const RouteFollower& routeFollower) {
    Vec2 previousPoint = routeFollower.getActivePathStart();
    for (Vec2 waypoint : routeFollower.getActivePath()) {
        DrawLineEx(ToRaylib(previousPoint), ToRaylib(waypoint), 3.0f, MAGENTA);
        DrawCircleV(ToRaylib(waypoint), 5.0f, MAGENTA);
        previousPoint = waypoint;
    }
}
