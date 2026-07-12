#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <raylib.h>
#include <math.h>
#include "cars.h"

#define MAX_AICARS 5

typedef struct {
    Vector2 pos;
    float rotation;   // heading, degrees, same convention as player
    float speed;      // constant forward speed
    float radius;     // collision radius
    int textureIndex; // which of cartextures[] to draw
    bool active;       // false once it has left the world / despawned
} AICar;

typedef enum {
    STATE_PLAYING,
    STATE_PAUSED,
    STATE_GAMEOVER
} GameState;

// Spawns (or respawns) a single AI car at a random valid position with a random heading/speed.
void SpawnSingleAICar(AICar *car, float worldwidth, float worldheight, float radius) {
    car->pos.x = 50.0f + (float)(rand() % (int)(worldwidth - 100.0f));
    car->pos.y = 50.0f + (float)(rand() % (int)(worldheight - 100.0f));
    car->rotation = (float)(rand() % 360);
    car->speed = 40.0f + (float)(rand() % 40); // pixels/sec, constant
    car->radius = radius;
    car->textureIndex = rand() % 4;
    car->active = true;
}

// (Re)spawns all AI cars at once - used for the initial fill and on restart.
void SpawnAICars(AICar aicars[], int count, float worldwidth, float worldheight, float radius) {
    for (int i = 0; i < count; i++) {
        SpawnSingleAICar(&aicars[i], worldwidth, worldheight, radius);
    }
}

int main()
{
    Vector2 pos={50.0f,50.0f};
    float rotation=0.0f;
    float speed=0.0f;
    vehicle playervehicle=GetVehiclePreset(4);
    
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    int gamewidth=1024;
    int gameheight=768;
    InitWindow(gamewidth,gameheight,"Urban Drift");
    RenderTexture2D target=LoadRenderTexture(gamewidth,gameheight);
    SetTextureFilter(target.texture,TEXTURE_FILTER_BILINEAR); //when zooming in, pixels are blended smoothly together
    SetTargetFPS(144);

    Texture2D cartextures[4];
    cartextures[0]=LoadTexture("assets/rickshaw.png");
    cartextures[1]=LoadTexture("assets/cng.png");
    cartextures[2]=LoadTexture("assets/auto.png");
    cartextures[3]=LoadTexture("assets/car.png");
    Texture2D currenttexture=cartextures[playervehicle.id-1];

    Texture2D mapbackground=LoadTexture("assets/hlw.png");
    SetTextureWrap(mapbackground, TEXTURE_WRAP_REPEAT);

    float worldwidth=1024.0f;
    float worldheight=3072.0f;

    Camera2D camera={0};
    camera.target=pos;
    camera.offset=(Vector2){(float)gamewidth/2.0f,(float)gameheight/2.0f}; 
    camera.rotation=0.0f;
    camera.zoom=1.0f;

    float playerRadius = (playervehicle.width + playervehicle.height) / 4.0f;

    srand((unsigned int)time(NULL));
    AICar aicars[MAX_AICARS];
    SpawnAICars(aicars, MAX_AICARS, worldwidth, worldheight, playerRadius);

    GameState gamestate = STATE_PLAYING;
    float flashTimer = 0.0f;     // counts down after a collision, drives the red flash
    const float FLASH_DURATION = 0.4f;

    float spawnTimer = 0.0f;         // counts up towards the next periodic respawn check
    const float SPAWN_INTERVAL = 3.0f; // every 3 seconds, try to refill one inactive slot

    // Play Again button, defined in game-canvas coordinates (1024x768), not window coordinates
    Rectangle playAgainButton = { gamewidth/2.0f - 100.0f, gameheight/2.0f + 40.0f, 200.0f, 60.0f };
    Rectangle resumeButton = { gamewidth/2.0f - 100.0f, gameheight/2.0f + 40.0f, 200.0f, 60.0f };

    Passenger currentpassenger={
        .position=(Vector2){400.0f,300.0f},
        .destination=(Vector2){900.0f,100.0f},
        .isspawned=true,
        .ispickedup=false,
        .interactionradius=40.0f
    };
    int money=0;

    SetExitKey(KEY_NULL); // ESC is repurposed for pause below, so stop it from closing the window
    
    while(!WindowShouldClose()){
        float dt = GetFrameTime();

        // Compute the letterbox scale/offset up front, since we need it both for
        // mapping mouse clicks (game-over button) and for the final draw
        float windowwidth=GetScreenWidth();
        float windowheight=GetScreenHeight();
        float scale=fminf(windowwidth/(float)gamewidth,windowheight/(float)gameheight);
        float viewWidth=(float)gamewidth*scale;
        float viewHeight=(float)gameheight*scale;
        float viewX=(windowwidth-viewWidth)*0.5f;
        float viewY=(windowheight-viewHeight)*0.5f;

        // ESC toggles pause, but only while actually playing or already paused (not on game-over screen)
        if (IsKeyPressed(KEY_ESCAPE)) {
            if (gamestate == STATE_PLAYING) gamestate = STATE_PAUSED;
            else if (gamestate == STATE_PAUSED) gamestate = STATE_PLAYING;
        }

        if (gamestate == STATE_PLAYING) {
            if (speed!=0.0f){
                if(IsKeyDown(KEY_D)) rotation+=0.5f;
                if(IsKeyDown(KEY_A)) rotation-=0.5f;
            }
            if(IsKeyDown(KEY_W) && speed<playervehicle.maxspeed && !IsKeyDown(KEY_S)) speed+=playervehicle.acceleration;
            if(IsKeyDown(KEY_S) && speed>-playervehicle.maxspeed && !IsKeyDown(KEY_W)) speed-=playervehicle.acceleration;
            if(IsKeyUp(KEY_W)&&IsKeyUp(KEY_S)&&speed!=0){
                if(speed>0) speed-=playervehicle.friction;
                if(speed<0) speed+=playervehicle.friction;
                if (speed<0.01f&&speed>-0.01f) speed=0.0f;
            }

            float moveX=sinf(rotation*DEG2RAD)*speed;
            float moveY=-cosf(rotation*DEG2RAD)*speed;
            float nextX=pos.x+moveX;
            float leftwall=playervehicle.width/2.0f;
            float rightwall=worldwidth-playervehicle.width/2.0f; 
            if(nextX<leftwall) pos.x=leftwall;
            else if(nextX>rightwall) pos.x=rightwall;
            else pos.x=nextX;
            float nextY=pos.y+moveY;
            float topwall=playervehicle.height/2.0f;
            float bottomwall=worldheight-playervehicle.height/2.0f; 
            if(nextY<topwall) pos.y=topwall;
            else if(nextY>bottomwall) pos.y=bottomwall;
            else pos.y=nextY;

            // --- PASSENGER PICKUP / DROPOFF ---
            if(currentpassenger.isspawned) {
                if(!currentpassenger.ispickedup){
                    if(CheckCollisionCircles(pos,playervehicle.width/3.0f,currentpassenger.position,currentpassenger.interactionradius) && speed==0.0f){
                        currentpassenger.ispickedup=true;
                    }
                }
                else{
                    if(CheckCollisionCircles(pos,playervehicle.width/3.0f,currentpassenger.destination,currentpassenger.interactionradius) && speed==0.0f){
                        currentpassenger.ispickedup=false;
                        currentpassenger.isspawned=false;
                        money+=100;
                    }
                }
            }

            // --- UPDATE AI CARS: move straight, despawn (disappear) at world edge ---
            for (int i = 0; i < MAX_AICARS; i++) {
                if (!aicars[i].active) continue;

                float aiMoveX = sinf(aicars[i].rotation*DEG2RAD)*aicars[i].speed*dt;
                float aiMoveY = -cosf(aicars[i].rotation*DEG2RAD)*aicars[i].speed*dt;
                float aiNextX = aicars[i].pos.x + aiMoveX;
                float aiNextY = aicars[i].pos.y + aiMoveY;

                if (aiNextX < aicars[i].radius || aiNextX > worldwidth - aicars[i].radius ||
                    aiNextY < aicars[i].radius || aiNextY > worldheight - aicars[i].radius) {
                    aicars[i].active = false; // hit the border: disappear instead of bouncing
                } else {
                    aicars[i].pos.x = aiNextX;
                    aicars[i].pos.y = aiNextY;
                }
            }

            // --- PERIODIC RESPAWN: every SPAWN_INTERVAL seconds, refill one inactive slot ---
            spawnTimer += dt;
            if (spawnTimer >= SPAWN_INTERVAL) {
                spawnTimer = 0.0f;
                for (int i = 0; i < MAX_AICARS; i++) {
                    if (!aicars[i].active) {
                        SpawnSingleAICar(&aicars[i], worldwidth, worldheight, playerRadius);
                        break; // only bring back one car per interval, not all at once
                    }
                }
            }

            // --- PLAYER vs AI CAR COLLISION (radius-based) ---
            for (int i = 0; i < MAX_AICARS; i++) {
                if (!aicars[i].active) continue;
                float dx = pos.x - aicars[i].pos.x;
                float dy = pos.y - aicars[i].pos.y;
                float distSq = dx*dx + dy*dy;
                float radiusSum = playerRadius + aicars[i].radius;
                if (distSq < radiusSum*radiusSum) {
                    speed = 0.0f;
                    gamestate = STATE_GAMEOVER;
                    flashTimer = FLASH_DURATION;
                    break;
                }
            }

            if(pos.x<(float)gamewidth/2.0f) camera.target.x=(float)gamewidth/2.0f;
            else if(pos.x>worldwidth-((float)gamewidth/2.0f)) camera.target.x=worldwidth-((float)gamewidth/2.0f);
            else camera.target.x = pos.x;
            if(pos.y<(float)gameheight/2.0f) camera.target.y=(float)gameheight/2.0f;
            else if(pos.y>worldheight-((float)gameheight/2.0f)) camera.target.y=worldheight-((float)gameheight/2.0f);
            else camera.target.y = pos.y;
        }
        else if (gamestate == STATE_PAUSED) {
            // Everything is frozen: no player input, no AI movement, no collision checks.
            // Just check whether the player clicked Resume.
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                Vector2 mouse = GetMousePosition();
                Vector2 gameMouse = { (mouse.x - viewX) / scale, (mouse.y - viewY) / scale };
                if (CheckCollisionPointRec(gameMouse, resumeButton)) {
                    gamestate = STATE_PLAYING;
                }
            }
        }
        else { // STATE_GAMEOVER
            if (flashTimer > 0.0f) flashTimer -= dt;
            if (flashTimer < 0.0f) flashTimer = 0.0f;

            // Check for "Play Again" click, converting mouse coords from window space
            // into game-canvas space to account for the letterboxed scaling
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                Vector2 mouse = GetMousePosition();
                Vector2 gameMouse = { (mouse.x - viewX) / scale, (mouse.y - viewY) / scale };
                if (CheckCollisionPointRec(gameMouse, playAgainButton)) {
                    // --- RESTART ---
                    pos = (Vector2){50.0f,50.0f};
                    rotation = 0.0f;
                    speed = 0.0f;
                    SpawnAICars(aicars, MAX_AICARS, worldwidth, worldheight, playerRadius);
                    gamestate = STATE_PLAYING;
                    flashTimer = 0.0f;
                    spawnTimer = 0.0f;
                    currentpassenger.position=(Vector2){400.0f,300.0f};
                    currentpassenger.destination=(Vector2){900.0f,100.0f};
                    currentpassenger.isspawned=true;
                    currentpassenger.ispickedup=false;
                    money=0;
                }
            }
        }

        // --- DRAWING TO CANVAS ---
        BeginTextureMode(target);
            ClearBackground(RAYWHITE);
            BeginMode2D(camera);

            Rectangle mapsource={0.0f,0.0f,worldwidth,worldheight};
            Rectangle mapdest={0.0f,0.0f,worldwidth,worldheight};
            DrawTexturePro(mapbackground,mapsource,mapdest,(Vector2){0.0f,0.0f},0.0f,WHITE);

            if(currentpassenger.isspawned){
                if(!currentpassenger.ispickedup){
                    DrawCircleV(currentpassenger.position,currentpassenger.interactionradius/3.0f,GREEN);
                    DrawCircleLinesV(currentpassenger.position,currentpassenger.interactionradius,GREEN);
                }
                else{
                    DrawCircleV(currentpassenger.destination,currentpassenger.interactionradius/3.0f,ORANGE);
                    DrawCircleLinesV(currentpassenger.destination,currentpassenger.interactionradius,RED);
                }
            }

            for (int i = 0; i < MAX_AICARS; i++) {
                if (!aicars[i].active) continue;
                Texture2D aitex = cartextures[aicars[i].textureIndex];
                DrawTexturePro(
                    aitex,
                    (Rectangle){0.0f,0.0f,(float)aitex.width,(float)aitex.height},
                    (Rectangle){aicars[i].pos.x,aicars[i].pos.y,playervehicle.width,playervehicle.height},
                    (Vector2){playervehicle.width/2.0f,playervehicle.height/2.0f},
                    aicars[i].rotation,
                    WHITE);
            }

            DrawTexturePro(
                currenttexture,
                (Rectangle){0.0f,0.0f,(float)currenttexture.width,(float)currenttexture.height},
                (Rectangle){pos.x,pos.y,playervehicle.width,playervehicle.height},
                (Vector2){playervehicle.width/2.0f,playervehicle.height/2.0f},
                rotation,
                WHITE);
                EndMode2D();

            // --- COLLISION FLASH EFFECT (drawn in screen space, over the whole canvas) ---
            if (flashTimer > 0.0f) {
                float alpha = flashTimer / FLASH_DURATION; // fades from 1 to 0
                DrawRectangle(0, 0, gamewidth, gameheight, Fade(RED, alpha * 0.6f));
            }

            // --- PAUSE MENU ---
            if (gamestate == STATE_PAUSED) {
                DrawRectangle(0, 0, gamewidth, gameheight, Fade(BLACK, 0.5f));

                const char* pausedText = "PAUSED";
                int fontSize = 60;
                int textWidth = MeasureText(pausedText, fontSize);
                DrawText(pausedText, gamewidth/2 - textWidth/2, gameheight/2 - 100, fontSize, WHITE);

                DrawRectangleRec(resumeButton, DARKGRAY);
                DrawRectangleLinesEx(resumeButton, 2, WHITE);
                const char* resumeText = "Resume";
                int rFontSize = 28;
                int rTextWidth = MeasureText(resumeText, rFontSize);
                DrawText(resumeText,
                    (int)(resumeButton.x + resumeButton.width/2 - rTextWidth/2),
                    (int)(resumeButton.y + resumeButton.height/2 - rFontSize/2),
                    rFontSize, WHITE);
            }

            // --- GAME OVER MENU ---
            if (gamestate == STATE_GAMEOVER) {
                DrawRectangle(0, 0, gamewidth, gameheight, Fade(BLACK, 0.5f));

                const char* gameOverText = "GAME OVER";
                int fontSize = 60;
                int textWidth = MeasureText(gameOverText, fontSize);
                DrawText(gameOverText, gamewidth/2 - textWidth/2, gameheight/2 - 100, fontSize, WHITE);

                DrawRectangleRec(playAgainButton, DARKGRAY);
                DrawRectangleLinesEx(playAgainButton, 2, WHITE);
                const char* playAgainText = "Play Again";
                int paFontSize = 28;
                int paTextWidth = MeasureText(playAgainText, paFontSize);
                DrawText(playAgainText,
                    (int)(playAgainButton.x + playAgainButton.width/2 - paTextWidth/2),
                    (int)(playAgainButton.y + playAgainButton.height/2 - paFontSize/2),
                    paFontSize, WHITE);
            }
        EndTextureMode();
        
        BeginDrawing();
            ClearBackground(BLACK); //black fills the remaining space

            DrawTexturePro(
                target.texture,
                (Rectangle){0,0,(float)target.texture.width,(float)-target.texture.height},
                (Rectangle){viewX,viewY,viewWidth,viewHeight}, // centered and scaled evenly
                (Vector2){0,0}, 
                0.0f,
                WHITE);

            DrawText(TextFormat("MONEY:%04d",money),20,20,30,BLUE);
            if(currentpassenger.isspawned && currentpassenger.ispickedup) DrawText("DESTINATION MARKED ON MAP",20,60,20,GOLD);
        EndDrawing();
    }
    for(int i=0;i<4;i++) UnloadTexture(cartextures[i]);
    UnloadTexture(mapbackground);
    UnloadRenderTexture(target);
    CloseWindow();
}
