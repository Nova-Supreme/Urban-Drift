#ifndef RENDER_H
#define RENDER_H

#include <raylib.h>
#include "player.h"
#include "cars.h"

// Draws the game world (map, passenger markers, vehicle) onto the render texture.
void Render_DrawWorld(RenderTexture2D target, Camera2D camera, Texture2D maptexture, float worldwidth, float worldheight, Passenger passenger, Player player);

// Draws the final scaled frame + HUD + pause overlay to the screen.
void Render_DrawScreen(RenderTexture2D target, int gamewidth, int gameheight, int money, Passenger passenger, bool paused);

#endif
