#ifndef RENDER_H
#define RENDER_H

#include <raylib.h>
#include "player.h"
#include "cars.h"
#include "enemy.h"
#include "road.h"

// Draws the game world (map, passenger markers, vehicles) onto the render texture.
// `showRoad` draws the road shapes as outlines so the road.c numbers can be
// matched to the asphalt (debug helper).
void Render_DrawWorld(RenderTexture2D target, Camera2D camera, Texture2D maptexture, float worldwidth, float worldheight, Passenger* passengers, int passengerCount, Player player, Enemy* enemies, int maxenemies, RoadNetwork* road, bool showRoad);

// Draws the final scaled frame + HUD + pause/game-over overlay to the screen.
// `overallSeconds` is the whole seconds of TOTAL run time, shown in the HUD
// (this run's clock keeps counting across levels until the CAR is unlocked).
// `money` is the current cash, `lives` the remaining lives, and `moneyMultiplier`
// the current fare multiplier (shown under the clock so the player sees it grow).
void Render_DrawScreen(RenderTexture2D target, int gamewidth, int gameheight, int money, int lives, Passenger* passengers, int passengerCount, bool paused, bool gameover, Vector2 playerpos, int overallSeconds, float moneyMultiplier);

#endif
