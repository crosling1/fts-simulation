#include "simulation/RouteFollower.h"

#include "rendering/RaylibGeometry.h"

void RouteFollower::draw() const {
    Vec2 previousPoint = activePathStart_;
    for (Vec2 waypoint : activePath_) {
        DrawLineEx(ToRaylib(previousPoint), ToRaylib(waypoint), 3.0f, MAGENTA);
        DrawCircleV(ToRaylib(waypoint), 5.0f, MAGENTA);
        previousPoint = waypoint;
    }
}
