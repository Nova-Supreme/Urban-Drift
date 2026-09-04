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
// `levelSeconds` is the whole seconds left on the level clock, shown in the HUD.
// `money` is the current cash and `lives` the remaining lives.
void Render_DrawScreen(RenderTexture2D target, int gamewidth, int gameheight, int money, int lives, Passenger* passengers, int passengerCount, bool paused, bool gameover, Vector2 playerpos, int levelSeconds);

// Draws the "LEVEL COMPLETE" screen (shown when time runs out): how much money
// was earned this level, plus a note about the next level / menu keys.
void Render_DrawWin(int gamewidth, int gameheight, int money, int levelStartMoney);

// Draws the settings menu: a "wipe all data" option with a confirmation prompt.
// `confirming` is true once the player has chosen to wipe and must press Y/N.
void Render_DrawSettings(int width, int height, bool confirming);

#endif
