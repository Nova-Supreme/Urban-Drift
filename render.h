#ifndef RENDER_H
#define RENDER_H

#include <raylib.h>
#include "player.h"
#include "cars.h"

// Draws the game world (map, passenger markers, vehicle) onto the render texture.
void Render_DrawWorld(RenderTexture2D target, Camera2D camera, Texture2D maptexture, float worldwidth, float worldheight, Passenger passenger, Player player);

// Draws the scaled game frame + HUD onto the screen.
// NOTE: no longer wraps BeginDrawing/EndDrawing and no longer draws a pause
// overlay - main.c now owns the frame's Begin/EndDrawing so screens like the
// pause menu can be layered on top within the same frame.
void Render_DrawScreen(RenderTexture2D target, int gamewidth, int gameheight, int money, Passenger passenger);

#endif
