#include "render.h"
#include <math.h>

// Draws one line of HUD text with a dark box behind it so it stays readable
// over any part of the map.
static void DrawHudText(int x, int y, int size, const char* text, Color color)
{
    int textWidth = MeasureText(text, size);
    DrawRectangle(x - 4, y - 2, textWidth + 8, size + 4, (Color){0,0,0,160});
    DrawText(text, x, y, size, color);
}

void Render_DrawWorld(RenderTexture2D target, Camera2D camera, Texture2D maptexture, float worldwidth, float worldheight, Passenger* passengers, int passengerCount, Player player, Enemy* enemies, int maxenemies, RoadNetwork* road, bool showRoad)
{
    BeginTextureMode(target);
        ClearBackground(RAYWHITE);
        BeginMode2D(camera);

            DrawTexturePro(
                maptexture,
                (Rectangle){0.0f,0.0f,(float)maptexture.width,(float)maptexture.height},
                (Rectangle){0.0f,0.0f,worldwidth,worldheight},
                (Vector2){0.0f,0.0f},
                0.0f,
                WHITE);

            // Debug helper: draw the road pieces as magenta outlines so the
            // numbers in road.c can be matched to the visible asphalt (F3).
            if(showRoad){
                for(int i=0;i<road->count;i++){
                    RoadSegment seg = road->segs[i];
                    if(seg.type == ROAD_RECT){
                        DrawRectangleLinesEx(seg.rect, 3.0f, MAGENTA);
                    }
                    else if(seg.type == ROAD_CIRCLE){
                        DrawCircleLinesV(seg.center, seg.radius, MAGENTA);
                        DrawCircleV(seg.center, 5.0f, MAGENTA);
                    }
                    else { // ROAD_SEMI: draw just the arc between the two angles
                        DrawCircleSectorLines(seg.center, seg.radius,
                                              seg.startAngle, seg.endAngle,
                                              32, MAGENTA);
                        DrawCircleV(seg.center, 5.0f, MAGENTA);
                    }
                }
            }

            // Draw every passenger: green = waiting to be picked up, orange
            // circle = the destination of a passenger you are carrying.
            for(int i=0;i<passengerCount;i++){
                Passenger* p = &passengers[i];
                if(!p->isspawned) continue;
                if(!p->ispickedup){
                    DrawCircleV(p->position,p->interactionradius/3.0f,GREEN);
                    DrawCircleLinesV(p->position,p->interactionradius,GREEN);
                }
                else{
                    DrawCircleV(p->destination,p->interactionradius/3.0f,ORANGE);
                    DrawCircleLinesV(p->destination,p->interactionradius,RED);
                }
            }

            DrawTexturePro(
                player.texture,
                (Rectangle){0.0f,0.0f,(float)player.texture.width,(float)player.texture.height},
                (Rectangle){player.pos.x,player.pos.y,player.veh.width,player.veh.height},
                (Vector2){player.veh.width/2.0f,player.veh.height/2.0f},
                player.rotation,
                WHITE);

            // Draw every active enemy car on top of the player.
            for (int i = 0; i < maxenemies; i++)
            {
                if (!enemies[i].active) continue;

                DrawTexturePro(
                    enemies[i].texture,
                    (Rectangle){0.0f,0.0f,(float)enemies[i].texture.width,(float)enemies[i].texture.height},
                    (Rectangle){enemies[i].pos.x,enemies[i].pos.y,enemies[i].width,enemies[i].height},
                    (Vector2){enemies[i].width/2.0f,enemies[i].height/2.0f},
                    enemies[i].rotation,
                    WHITE);
            }

        EndMode2D();
    EndTextureMode();
}

void Render_DrawScreen(RenderTexture2D target, int gamewidth, int gameheight, int money, int lives, Passenger* passengers, int passengerCount, bool paused, bool gameover, Vector2 playerpos, int overallSeconds, float moneyMultiplier)
{
    BeginDrawing();
        ClearBackground(BLACK);
        float windowwidth = GetScreenWidth();
        float windowheight = GetScreenHeight();

        float scale = fminf(windowwidth/(float)gamewidth, windowheight/(float)gameheight);
        float viewwidth = (float)gamewidth*scale;
        float viewheight = (float)gameheight*scale;
        float viewX = (windowwidth-viewwidth)/2.0f;
        float viewY = (windowheight-viewheight)/2.0f;

        DrawTexturePro(
            target.texture,
            (Rectangle){0,0,(float)target.texture.width,(float)-target.texture.height},
            (Rectangle){viewX,viewY,viewwidth,viewheight},
            (Vector2){0,0},
            0.0f,
            WHITE);

        // HUD: money and lives top-left, the total run time below them, and the fare
        // multiplier under that (it climbs 0.1 with every delivery, back to 1.0
        // on each new round). The player's coordinates sit top-right, with
        // every carried passenger's destination coordinates listed right
        // underneath. Each has a dark box.
        DrawHudText(20, 20, 30, TextFormat("MONEY:%04d", money), BLUE);
        DrawHudText(20, 60, 30, TextFormat("LIVES:%d", lives), RED);
        // The overall run clock as minutes:seconds (runs across whole levels).
        DrawHudText(20, 100, 30, TextFormat("TIME:%d:%02d", overallSeconds/60, overallSeconds%60), GOLD);
        // Show the current fare multiplier, e.g. "FARE:x2.4".
        DrawHudText(20, 140, 22, TextFormat("FARE:x%.1f", moneyMultiplier), GREEN);

        // Top-right: the player's own position, then a line for each passenger
        // currently being carried showing their destination coordinates.
        int coordSize = 25;
        const char* coord = TextFormat("X:%.0f  Y:%.0f", playerpos.x, playerpos.y);
        DrawHudText((int)windowwidth - (int)MeasureText(coord, coordSize) - 20, 20, coordSize, coord, WHITE);

        int destY = 55; // stack destinations below the player's coordinates
        int destNum = 1;
        for(int i=0;i<passengerCount;i++){
            if(!passengers[i].isspawned || !passengers[i].ispickedup) continue;
            const char* dest = TextFormat("D%d X:%.0f  Y:%.0f", destNum,
                                          passengers[i].destination.x,
                                          passengers[i].destination.y);
            int destWidth = MeasureText(dest, coordSize);
            DrawHudText((int)windowwidth - destWidth - 20, destY, coordSize, dest, GOLD);
            destY += 40;
            destNum++;
        }

        if(paused){
            DrawRectangle(0,0,windowwidth,windowheight,(Color){0,0,0,150});
            const char* pausedtext="PAUSED";
            int textwidth=MeasureText(pausedtext,50);
            DrawText(pausedtext,(windowwidth-textwidth)/2,windowheight/2-25,50,WHITE);

            const char* hint="Press ESC to resume   |   M for menu";
            int hintwidth=MeasureText(hint,20);
            DrawText(hint,(windowwidth-hintwidth)/2,windowheight/2+30,20,GRAY);
        }

        if(gameover){
            // Dark overlay + a centered "game over" box with the result.
            DrawRectangle(0,0,windowwidth,windowheight,(Color){0,0,0,200});

            const char* title="GAME OVER";
            int titlewidth=MeasureText(title,70);
            DrawText(title,(windowwidth-titlewidth)/2,windowheight/2-120,70,RED);

            const char* result=TextFormat("You earned $%d",money);
            int resultwidth=MeasureText(result,30);
            DrawText(result,(windowwidth-resultwidth)/2,windowheight/2-20,30,WHITE);

            const char* prompt="Press R to restart   |   ESC for menu";
            int promptwidth=MeasureText(prompt,25);
            DrawText(prompt,(windowwidth-promptwidth)/2,windowheight/2+50,25,GRAY);
        }

    EndDrawing();
}


