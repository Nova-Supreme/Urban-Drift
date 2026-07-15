#include "render.h"
#include <math.h>

void Render_DrawWorld(RenderTexture2D target, Camera2D camera, Texture2D maptexture, float worldwidth, float worldheight, Passenger passenger, Player player)
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

            if(passenger.isspawned){
                if(!passenger.ispickedup){
                    DrawCircleV(passenger.position,passenger.interactionradius/3.0f,GREEN);
                    DrawCircleLinesV(passenger.position,passenger.interactionradius,GREEN);
                }
                else{
                    DrawCircleV(passenger.destination,passenger.interactionradius/3.0f,ORANGE);
                    DrawCircleLinesV(passenger.destination,passenger.interactionradius,RED);
                }
            }

            DrawTexturePro(
                player.texture,
                (Rectangle){0.0f,0.0f,(float)player.texture.width,(float)player.texture.height},
                (Rectangle){player.pos.x,player.pos.y,player.veh.width,player.veh.height},
                (Vector2){player.veh.width/2.0f,player.veh.height/2.0f},
                player.rotation,
                WHITE);

        EndMode2D();
    EndTextureMode();
}

void Render_DrawScreen(RenderTexture2D target, int gamewidth, int gameheight, int money, Passenger passenger, bool paused)
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

        DrawText(TextFormat("MONEY:%04d",money),20,20,30,BLUE);
        if(passenger.isspawned && passenger.ispickedup) DrawText("DESTINATION MARKED ON MAP",20,60,20,GOLD);

        if(paused){
            DrawRectangle(0,0,windowwidth,windowheight,(Color){0,0,0,150});
            const char* pausedtext="PAUSED";
            int textwidth=MeasureText(pausedtext,50);
            DrawText(pausedtext,(windowwidth-textwidth)/2,windowheight/2-25,50,WHITE);
        }

    EndDrawing();
}
