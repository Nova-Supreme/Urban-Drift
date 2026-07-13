#ifndef PAUSEMENU_H
#define PAUSEMENU_H

#include <raylib.h>
#include <stdbool.h>

// Draws the "PAUSED" overlay and the Resume button. gamewidth/gameheight are the
// game-canvas dimensions (not the actual window size), since this is drawn inside
// the render texture, same coordinate space as the rest of the game world's HUD.
void DrawPauseMenu(int gamewidth, int gameheight, Rectangle resumeButton);

// Returns true exactly once, on the frame the player left-clicks inside resumeButton.
// mouseX/mouseY should be the raw window-space mouse position (e.g. from GetMousePosition());
// viewX/viewY/scale are the same letterbox values used to draw the final scaled canvas,
// needed here to map the click back into game-canvas coordinates.
bool IsResumeButtonClicked(Rectangle resumeButton, float mouseX, float mouseY, float viewX, float viewY, float scale);

#endif
