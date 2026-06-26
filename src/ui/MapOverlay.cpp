#include "ui/MapOverlay.h"

#include "raylib.h"

void DrawMapOverlay() {
    DrawText("Robot Logistics Map", 20, 20, 28, DARKGRAY);
    DrawText("A: Pickup lager", 20, 60, 18, DARKGREEN);
    DrawText("B: Delivery lager", 20, 84, 18, MAROON);
    DrawText("L1~L6: Lager / warehouses", 20, 108, 18, BLUE);
    DrawText("Orange circle: Robot start", 20, 132, 18, ORANGE);
    DrawText("Gray rectangles: road network to each lager", 20, 156, 18, DARKGRAY);
    DrawText("C: Charging station", 20, 180, 18, DARKPURPLE);
}