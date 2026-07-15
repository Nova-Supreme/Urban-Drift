#include <stdio.h>
#include <raylib.h>
#include <math.h>
#include "cars.h"
#include "intro.h"
#include "player.h"
#include "passenger.h"
#include "render.h"
#include "gamestate.h"
#include "menu.h"

int main()
{
    // ------------------------------------------------------------
    // Gameplay state - identical to the previous version, only the
    // "paused" bool is gone since STATE_PAUSE now covers that.
    // ------------------------------------------------------------
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

    Passenger currentpassenger={
        .position=(Vector2){490.0f,160.0f},
        .destination=(Vector2){1750.0f,1200.0f},
        .isspawned=true,
        .ispickedup=false,
        .interactionradius=40.0f
    };

    int money=0;
    int score=0;

    // ------------------------------------------------------------
    // Navigation state
    // ------------------------------------------------------------
    GameState gameState = STATE_CUTSCENE;

    while(!WindowShouldClose() && gameState != STATE_QUIT){

        switch(gameState){

            // ---------------------------------------------------------
            // Intro cutscene - unmodified logic (Intro_HandleInput/Draw
            // are exactly as before), we just react to intro.over here.
            // ---------------------------------------------------------
            case STATE_CUTSCENE:
            {
                Intro_HandleInput(&intro);
                if(intro.over){
                    gameState = STATE_MAIN_MENU;
                    Menu_ResetMainMenu();
                }

                BeginDrawing();
                    ClearBackground(BLACK);
                    Intro_Draw(&intro, GetScreenWidth(), GetScreenHeight());
                EndDrawing();
            } break;

            // ---------------------------------------------------------
            case STATE_MAIN_MENU:
            {
                GameState next = UpdateMainMenu();
                if(next == STATE_LEVEL_SELECT) Menu_ResetLevelSelect();
                gameState = next;

                BeginDrawing();
                    ClearBackground(BLACK);
                    DrawMainMenu(GetScreenWidth(), GetScreenHeight());
                EndDrawing();
            } break;

            // ---------------------------------------------------------
            case STATE_LEVEL_SELECT:
            {
                gameState = UpdateLevelSelect();

                BeginDrawing();
                    ClearBackground(BLACK);
                    DrawLevelSelect(GetScreenWidth(), GetScreenHeight());
                EndDrawing();
            } break;

            // ---------------------------------------------------------
            case STATE_OPTIONS:
            {
                gameState = UpdateOptions();

                BeginDrawing();
                    ClearBackground(BLACK);
                    DrawOptions(GetScreenWidth(), GetScreenHeight());
                EndDrawing();
            } break;

            // ---------------------------------------------------------
            case STATE_CONTROLS:
            {
                gameState = UpdateControls();

                BeginDrawing();
                    ClearBackground(BLACK);
                    DrawControls(GetScreenWidth(), GetScreenHeight());
                EndDrawing();
            } break;

            // ---------------------------------------------------------
            // Gameplay - movement/camera/passenger logic below is byte
            // for byte the same as before, just relocated into this case.
            // ---------------------------------------------------------
            case STATE_GAME:
            {
                if(IsKeyPressed(KEY_ESCAPE)){
                    gameState = STATE_PAUSE;
                    Menu_ResetPauseMenu();
                }
                else{
                    Player_HandleInput(&player);
                    Player_UpdatePosition(&player, worldwidth, worldheight);
                    Passenger_Update(&currentpassenger, player.pos, player.veh.width, player.speed, &money);

                    if(player.pos.x<(float)gamewidth/2.0f) camera.target.x=(float)gamewidth/2.0f;
                    else if(player.pos.x>worldwidth-((float)gamewidth/2.0f)) camera.target.x=worldwidth-((float)gamewidth/2.0f);
                    else camera.target.x=player.pos.x;
                    if(player.pos.y<(float)gameheight/2.0f) camera.target.y=(float)gameheight/2.0f;
                    else if(player.pos.y>worldheight-((float)gameheight/2.0f)) camera.target.y=worldheight-((float)gameheight/2.0f);
                    else camera.target.y=player.pos.y;
                }

                Render_DrawWorld(target, camera, maptexture, worldwidth, worldheight, currentpassenger, player);

                BeginDrawing();
                    ClearBackground(BLACK);
                    Render_DrawScreen(target, gamewidth, gameheight, money, currentpassenger);
                EndDrawing();
            } break;

            // ---------------------------------------------------------
            // Pause - world is drawn frozen (no updates called) behind
            // the pause menu overlay.
            // ---------------------------------------------------------
            case STATE_PAUSE:
            {
                GameState next = UpdatePauseMenu();
                if(next == STATE_MAIN_MENU) Menu_ResetMainMenu();
                gameState = next;

                Render_DrawWorld(target, camera, maptexture, worldwidth, worldheight, currentpassenger, player);

                BeginDrawing();
                    ClearBackground(BLACK);
                    Render_DrawScreen(target, gamewidth, gameheight, money, currentpassenger);
                    DrawPauseMenu(GetScreenWidth(), GetScreenHeight());
                EndDrawing();
            } break;

            default: break;
        }
    }

    for(int i=0;i<4;i++) UnloadTexture(car[i]);
    UnloadTexture(map[0]);
    UnloadRenderTexture(target);
    CloseWindow();

    return 0;
}
