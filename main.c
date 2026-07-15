#include <stdio.h>
#include <raylib.h>
#include <math.h>
#include "cars.h"
#include "intro.h"
#include "player.h"
#include "passenger.h"
#include "render.h"

int main()
{
    Player player = {0};
    player.pos = (Vector2){170.0f,350.0f};
    player.rotation = 90.0f;
    player.targetRotation = 90.0f;
    player.speed = 0.0f;
    player.veh = GetVehiclePreset(1);

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    int gamewidth=1024;
    int gameheight=768;
    InitWindow(gamewidth,gameheight,"Urban Drift");
    RenderTexture2D target=LoadRenderTexture(gamewidth,gameheight); //creates virtual hidden canvas
    SetTextureFilter(target.texture,TEXTURE_FILTER_BILINEAR);
    SetTargetFPS(144);

    Intro intro;
    Intro_Load(&intro);

    Texture2D car[4];
    car[0]=LoadTexture("assets/rickshaw.png");
    car[1]=LoadTexture("assets/cng.png");
    car[2]=LoadTexture("assets/auto.png");
    car[3]=LoadTexture("assets/car.png");
    player.texture = car[player.veh.id-1];

    Texture2D map[5];
    map[0]=LoadTexture("assets/map0.png");
    Texture2D maptexture=map[0];

    float worldwidth=1024.0f*2.0f;
    float worldheight=768.0f*2.0f;

    Camera2D camera={0};
    camera.target=player.pos;
    camera.offset=(Vector2){(float)gamewidth/2.0f,(float)gameheight/2.0f};
    camera.rotation=0.0f;
    camera.zoom=1.0f;

    SetExitKey(KEY_NULL);
    bool paused=false;

    Passenger currentpassenger={
        .position=(Vector2){490.0f,160.0f},
        .destination=(Vector2){1750.0f,1200.0f},
        .isspawned=true,
        .ispickedup=false,
        .interactionradius=40.0f
    };

    int money=0;
    int score=0;

    while(!WindowShouldClose()){
        if(!intro.over){
            Intro_HandleInput(&intro);

            BeginDrawing();
            ClearBackground(BLACK);
            Intro_Draw(&intro, GetScreenWidth(), GetScreenHeight());
            EndDrawing();
            continue; //skip the rest of the loop entirely while backstory is showing
        }

        if(IsKeyPressed(KEY_ESCAPE)) paused=!paused;
        if(!paused){
            Player_HandleInput(&player);
        }

        // NOTE: preserved from the original file - movement/passenger/camera updates
        // run every frame regardless of `paused`, only input handling is gated.
        Player_UpdatePosition(&player, worldwidth, worldheight);
        Passenger_Update(&currentpassenger, player.pos, player.veh.width, player.speed, &money);

        if(player.pos.x<(float)gamewidth/2.0f) camera.target.x=(float)gamewidth/2.0f;
        else if(player.pos.x>worldwidth-((float)gamewidth/2.0f)) camera.target.x=worldwidth-((float)gamewidth/2.0f);
        else camera.target.x=player.pos.x;
        if(player.pos.y<(float)gameheight/2.0f) camera.target.y=(float)gameheight/2.0f;
        else if(player.pos.y>worldheight-((float)gameheight/2.0f)) camera.target.y=worldheight-((float)gameheight/2.0f);
        else camera.target.y=player.pos.y;

        Render_DrawWorld(target, camera, maptexture, worldwidth, worldheight, currentpassenger, player);
        Render_DrawScreen(target, gamewidth, gameheight, money, currentpassenger, paused);
    }

    for(int i=0;i<4;i++) UnloadTexture(car[i]);
    UnloadTexture(map[0]);
    UnloadRenderTexture(target);
    CloseWindow();

    return 0;
}
