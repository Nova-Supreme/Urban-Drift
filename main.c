#include <stdio.h>
#include <raylib.h>
#include <math.h>
#include "cars.h"
#include "intro.h"
#include "player.h"
#include "passenger.h"
#include "render.h"
#include "road.h"
#include "enemy.h"
#include "title.h"
#include "save.h"

// Set to 0 to re-enable enemy traffic. Kept off while road segments are being
// fine-tuned so the player can drive without being killed by enemies.
#define ENEMIES_DISABLED 1

// Which screen the game is showing right now.
typedef enum {
    STATE_INTRO,     // the opening slideshow (shown once, on launch)
    STATE_TITLE,     // the main menu
    STATE_VEHICLES,  // choosing which car to drive
    STATE_PLAYING,   // driving round
    STATE_OVER,      // the "game over" screen
    STATE_WIN,       // the "level complete" screen
    STATE_SETTINGS   // the settings menu (wipe saved data)
} GameState;

// How many seconds the player has to complete a level. The first map (map0)
// is timed at 5 minutes.
#define LEVEL_TIME_SECONDS 300.0f

// Switches the player's car to `vehicleId` and swaps its picture.
static void SetVehicle(Player* player, Texture2D* carTextures, int vehicleId)
{
    player->veh = GetVehiclePreset(vehicleId);
    player->texture = carTextures[vehicleId - 1];
}

// Puts everything back to the very start of a fresh round. Used by both the
// "PLAY" menu option and the "R" restart on the game-over screen.
// NOTE: the player's money is NOT reset here - it is a running total that is
// carried from the save file and only grows with fares (or shrinks when
// buying cars), so it survives from one round to the next.
static void ResetRun(Player* player, int* lives, int* score,
                     float* hurtTimer, Enemy* enemies, float* spawnTimer,
                     Passenger* passengers, int passengerCount, Intro* intro)
{
    player->pos = (Vector2){170.0f,350.0f};
    player->rotation = 90.0f;
    player->targetRotation = 90.0f;
    player->speed = 0.0f;

    *lives = 5;
    *score = 0;
    *hurtTimer = 0.0f;

    for(int i=0;i<MAX_ENEMIES;i++) enemies[i].active = false;
    *spawnTimer = 0.0f;

    // Clear any existing passengers. A brand-new one is spawned on a random
    // road point shortly after the level starts (see the PLAYING loop), and a
    // fresh one keeps appearing as others are dropped off.
    for(int i=0;i<passengerCount;i++){
        passengers[i].isspawned = false;
        passengers[i].ispickedup = false;
    }

    // The opening slideshow already played on launch, so a new round jumps
    // straight into the driving instead of replaying the story.
    intro->over = true;
}

// Sets up the bits of state that start fresh with each level: how long before
// the first passenger appears, how much time is left on the clock, and the
// money figure we later subtract to show "earned this level".
static void PrepareLevel(float* passengerTimer, float* levelTime, int* levelStartMoney, int totalMoney)
{
    *passengerTimer = 2.0f;
    *levelTime = LEVEL_TIME_SECONDS;
    *levelStartMoney = totalMoney;
}

// Saves the player's progress so "CONTINUE" can pick it up later. `active`
// should be 1 when the round is still going (so Continue works) and 0 when the
// player lost (so Continue is disabled).
static void SaveProgress(int money, int vehicleId, int ownedMask, int active)
{
    Save_Write((SaveData){money, vehicleId, ownedMask}, active);
}

int main()
{
    // ---- Game window setup ----
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    int gamewidth=1024;
    int gameheight=768;
    InitWindow(gamewidth,gameheight,"Urban Drift");
    RenderTexture2D target=LoadRenderTexture(gamewidth,gameheight); //creates virtual hidden canvas
    SetTextureFilter(target.texture,TEXTURE_FILTER_BILINEAR);
    SetTargetFPS(144);
    SetExitKey(KEY_NULL);

    // ---- Textures ----
    Texture2D car[4];
    car[0]=LoadTexture("assets/rickshaw.png");
    car[1]=LoadTexture("assets/cng.png");
    car[2]=LoadTexture("assets/auto.png");
    car[3]=LoadTexture("assets/car.png");

    Texture2D map[5];
    map[0]=LoadTexture("assets/map0.png");
    Texture2D maptexture=map[0];

    // ---- The player's car (starts as the rickshaw) ----
    Player player = {0};
    SetVehicle(&player, car, 1);

    // ---- The world (map, camera, road, enemies) ----
    float worldwidth=1024.0f*2.0f;
    float worldheight=768.0f*2.0f;

    Camera2D camera={0};
    camera.target=player.pos;
    camera.offset=(Vector2){(float)gamewidth/2.0f,(float)gameheight/2.0f};
    camera.rotation=0.0f;
    camera.zoom=1.0f;

    RoadNetwork road;
    Road_Load(&road);

    Enemy enemies[MAX_ENEMIES] = {0};
    float spawnTimer = 0.0f;
#if !ENEMIES_DISABLED
    const float spawnInterval = 2.0f; // a new enemy appears every 2 seconds
    int routeLength = Enemy_RouteLength();
#endif

    // ---- The passengers for the round ----
    // Several can be on the road (and carried) at once, so it's an array.
    Passenger passengers[MAX_PASSENGERS] = {0};

    // ---- Run state (score, lives, hurt window) ----
    // money and ownedMask persist between runs (from the save file). Only the
    // rickshaw (id 1) is owned at the very beginning.
    // savedVehicleId is the car the LAST round was driven with - CONTINUE uses
    // it, so changing cars in the menu never changes a continued run.
    int money          = 0;
    int ownedMask      = 1;
    int savedVehicleId = 1;
    int score          = 0;
    int lives          = 5;
#if !ENEMIES_DISABLED
    const float hurtTime=1.0f;
#endif
    float hurtTimer=0.0f;
    bool paused=false;
    bool confirmWipe=false; // true when the settings screen is asking to confirm

    // Timer before the next passenger appears (they keep coming one after
    // another while you drive), how much time is left on the level clock, and
    // the money when the level started (so we can show "earned this level").
    float passengerTimer=0.0f;
    float levelTime=LEVEL_TIME_SECONDS;
    int levelStartMoney=0;

    // Pull the saved money and owned cars into memory if a save already exists.
    SaveData saved;
    if(Save_Load(&saved)){
        money          = saved.money;
        ownedMask      = saved.ownedMask;
        savedVehicleId = saved.savedVehicleId;
    }

    // ---- Screen flow ----
    Intro intro;
    intro.over = false;
    Intro_Load(&intro);        // prepare the opening slideshow
    TitleScreen title;
    Title_Init(&title);
    GameState state = STATE_INTRO; // the slideshow is the very first screen
    bool quit = false;

    // Whether to draw the road shapes on screen (press F3 to toggle). This is
    // a helper for editing road.c so the shapes can be matched to the asphalt.
    bool showRoad = false;

    while(!quit && !WindowShouldClose()){

        // ---------------- OPENING SLIDESHOW ----------------
        // The story plays once at the very start; ESC or space through it,
        // then we move on to the title screen.
        if(state == STATE_INTRO){
            Intro_HandleInput(&intro);

            BeginDrawing();
            ClearBackground(BLACK);
            Intro_Draw(&intro, GetScreenWidth(), GetScreenHeight());
            EndDrawing();

            if(intro.over) state = STATE_TITLE; // slideshow finished
            continue;
        }

        // ---------------- TITLE SCREEN ----------------
        if(state == STATE_TITLE){
            // Pick what the player chose, if anything.
            int choice = Title_HandleMenu(&title);
            if(choice != -1){
                if(choice == TITLE_PLAY){
                    ResetRun(&player, &lives, &score, &hurtTimer,
                             enemies, &spawnTimer, passengers, MAX_PASSENGERS, &intro);
                    PrepareLevel(&passengerTimer, &levelTime, &levelStartMoney, money);
                    paused = false;
                    state = STATE_PLAYING;
                }
                else if(choice == TITLE_VEHICLES){
                    state = STATE_VEHICLES;
                }
                else if(choice == TITLE_SETTINGS){
                    confirmWipe = false; // start at the settings menu, not confirm
                    state = STATE_SETTINGS;
                }
                else if(choice == TITLE_QUIT){
                    quit = true;
                }
            }

            BeginDrawing();
            ClearBackground(BLACK);
            Title_DrawMenu(&title, GetScreenWidth(), GetScreenHeight());
            EndDrawing();
            continue;
        }

        // ---------------- VEHICLE SELECT ----------------
        if(state == STATE_VEHICLES){
            int action = Title_HandleVehicles(&title);
            if(action == VEHICLE_BACK){
                state = STATE_TITLE; // ESC: go back without changing anything
            }
            else if(action == VEHICLE_CHOOSE){
                int id   = title.vehicleIndex + 1; // highlighted car (1..4)
                int cost = GetVehiclePreset(id).cost;

                if(ownedMask & (1 << (id - 1))){
                    // Already owned -> make it the car used by the next PLAY.
                    SetVehicle(&player, car, id);
                    state = STATE_TITLE;
                }
                else if(money >= cost){
                    // Buy it: spend the money, mark it owned, and save progress.
                    // Purchases don't start or end a run, so keep the saved
                    // round exactly as active (continue-able) as it was.
                    money -= cost;
                    ownedMask |= (1 << (id - 1));
                    SaveProgress(money, savedVehicleId, ownedMask, saved.active);
                }
                // else: cannot afford it - simply nothing happens.
            }

            BeginDrawing();
            ClearBackground(BLACK);
            Title_DrawVehicles(&title, GetScreenWidth(), GetScreenHeight(), car, player.veh.id, money, ownedMask);
            EndDrawing();
            continue;
        }

        // ---------------- SETTINGS SCREEN ----------------
        // Offers a way to wipe all saved data and start completely fresh.
        // ENTER picks the wipe option, then a Y/N confirmation asks again
        // before anything is actually deleted.
        if(state == STATE_SETTINGS){
            if(confirmWipe){
                // We're asking "are you sure?": Y wipes, N or ESC cancels.
                if(IsKeyPressed(KEY_Y)){
                    Save_Delete();                    // remove the save file on disk
                    money = 0;                        // reset in-memory progress too
                    ownedMask = 1;                    // back to only the starter car
                    savedVehicleId = 1;
                    SetVehicle(&player, car, 1);
                    confirmWipe = false;
                    state = STATE_TITLE;
                }
                if(IsKeyPressed(KEY_N) || IsKeyPressed(KEY_ESCAPE)) confirmWipe = false;
            }
            else{
                // Plain settings menu: ENTER offers to wipe, ESC goes back.
                if(IsKeyPressed(KEY_ENTER)) confirmWipe = true;
                if(IsKeyPressed(KEY_ESCAPE)) state = STATE_TITLE;
            }

            BeginDrawing();
            ClearBackground(BLACK);
            Render_DrawSettings(GetScreenWidth(), GetScreenHeight(), confirmWipe);
            EndDrawing();
            continue;
        }

        // ---------------- GAME OVER SCREEN ----------------
        if(state == STATE_OVER){
            if(IsKeyPressed(KEY_R)){
                ResetRun(&player, &lives, &score, &hurtTimer,
                         enemies, &spawnTimer, passengers, MAX_PASSENGERS, &intro);
                PrepareLevel(&passengerTimer, &levelTime, &levelStartMoney, money);
                paused = false;
                state = STATE_PLAYING;
            }
            if(IsKeyPressed(KEY_ESCAPE)) state = STATE_TITLE; // back to menu

            Render_DrawWorld(target, camera, maptexture, worldwidth, worldheight, passengers, MAX_PASSENGERS, player, enemies, MAX_ENEMIES, &road, showRoad);
            Render_DrawScreen(target, gamewidth, gameheight, money, lives, passengers, MAX_PASSENGERS, false, true, player.pos, (int)levelTime);
            continue;
        }

        // ---------------- LEVEL COMPLETE SCREEN ----------------
        // Shown when the player survives until the whole level time is used up.
        // N moves to the next level; ESC returns to the menu.
        if(state == STATE_WIN){
            if(IsKeyPressed(KEY_N)){
                ResetRun(&player, &lives, &score, &hurtTimer,
                         enemies, &spawnTimer, passengers, MAX_PASSENGERS, &intro);
                PrepareLevel(&passengerTimer, &levelTime, &levelStartMoney, money);
                paused = false;
                state = STATE_PLAYING;
                // NOTE: only map0 exists, so the "next level" reuses map0 for
                // now. To add more maps, load map[n] into maptexture here and
                // pick the next asset before SetVehicle/ResetRun.
            }
            if(IsKeyPressed(KEY_ESCAPE)) state = STATE_TITLE; // back to menu

            Render_DrawWorld(target, camera, maptexture, worldwidth, worldheight, passengers, MAX_PASSENGERS, player, enemies, MAX_ENEMIES, &road, showRoad);
            Render_DrawWin(gamewidth, gameheight, money, levelStartMoney);
            continue;
        }

        // ---------------- PLAYING SCREEN ----------------
        // ESC toggles the pause overlay; M on the pause screen returns to menu.
        if(IsKeyPressed(KEY_ESCAPE)) paused = !paused;
        if(paused && IsKeyPressed(KEY_M)) {
            savedVehicleId = player.veh.id;             // remember the car being driven
            SaveProgress(money, savedVehicleId, ownedMask, 1); // still going - can continue
            paused = false;
            state = STATE_TITLE;
        }

        // While paused, the whole world freezes: no input, no movement, no
        // enemies, no crashing, nothing. Only the pause/quit keys above work.
        if(!paused){
            Player_HandleInput(&player);
            if(IsKeyPressed(KEY_F3)) showRoad = !showRoad;
            Player_UpdatePosition(&player, &road);
            // A passenger is picked up (up to the car's maxpassengers) or
            // dropped off at its destination each frame.
            Passenger_Update(passengers, MAX_PASSENGERS, player.pos, player.veh.width, player.speed, &money, player.veh.maxpassengers);

            // Keep a steady flow: whenever any passenger is missing (fresh
            // level or one was just dropped off), wait a moment and then place
            // a brand-new one in the first free slot.
            bool missingPassenger = false;
            for(int i=0;i<MAX_PASSENGERS;i++){
                if(!passengers[i].isspawned){ missingPassenger = true; break; }
            }
            if(missingPassenger){
                passengerTimer -= GetFrameTime();
                if(passengerTimer <= 0.0f){
                    for(int i=0;i<MAX_PASSENGERS;i++){
                        if(!passengers[i].isspawned){
                            Passenger_SpawnRandom(&passengers[i], &road, worldwidth, worldheight);
                            break; // only one new passenger per tick
                        }
                    }
                    passengerTimer = 4.0f; // next one comes a few seconds later
                }
            }

            #if !ENEMIES_DISABLED
            // Spawn a new enemy every few seconds, up to the MAX_ENEMIES limit.
            spawnTimer += GetFrameTime();
            if(spawnTimer >= spawnInterval){
                spawnTimer = 0.0f;
                // place it at a random waypoint on the route
                Enemy_Spawn(enemies, MAX_ENEMIES, GetRandomValue(0, routeLength-1), car[GetRandomValue(0,3)]);
            }

            // Move all the enemy cars along their route.
            Enemy_Update(enemies, MAX_ENEMIES);

            // Count down the "hurt" window.
            if(hurtTimer > 0.0f) hurtTimer -= GetFrameTime();

            // Check whether the player crashed into any enemy car.
            if(hurtTimer <= 0.0f){
                for(int i=0;i<MAX_ENEMIES;i++){
                    if(!enemies[i].active) continue;
                    if(CheckCollisionCircles(player.pos, player.veh.width/2.0f, enemies[i].pos, enemies[i].width/2.0f)){
                        lives--;                 // the crash costs one life
                        hurtTimer = hurtTime;    // restart the invulnerability window
                        score -= 10;             // and a small score penalty
                        if(score < 0) score = 0;
                        break;                   // only one crash is counted per frame
                    }
                }
            }
#endif

            // Count down the level clock. When time runs out the player has
            // finished the level - save progress and show the win screen.
            levelTime -= GetFrameTime();
            if(levelTime <= 0.0f){
                levelTime = 0.0f;
                savedVehicleId = player.veh.id;             // remember the car being driven
                SaveProgress(money, savedVehicleId, ownedMask, 1); // won - can continue
                state = STATE_WIN;
            }

            // When lives hit zero the round is over. Progress must be saved so
            // the player keeps the money/cars they earned, but the saved round
            // is marked as "over" so CONTINUE is disabled (you can't resume a
            // lost round - you restart with R instead).
            if(lives <= 0){
                savedVehicleId = player.veh.id;             // remember the car being driven
                SaveProgress(money, savedVehicleId, ownedMask, 0); // active = 0 (dead)
                state = STATE_OVER;
            }
        }

        // Keep the camera centred on the player, but not past the world edges.
        if(player.pos.x<(float)gamewidth/2.0f) camera.target.x=(float)gamewidth/2.0f;
        else if(player.pos.x>worldwidth-((float)gamewidth/2.0f)) camera.target.x=worldwidth-((float)gamewidth/2.0f);
        else camera.target.x=player.pos.x;
        if(player.pos.y<(float)gameheight/2.0f) camera.target.y=(float)gameheight/2.0f;
        else if(player.pos.y>worldheight-((float)gameheight/2.0f)) camera.target.y=worldheight-((float)gameheight/2.0f);
        else camera.target.y=player.pos.y;

        Render_DrawWorld(target, camera, maptexture, worldwidth, worldheight, passengers, MAX_PASSENGERS, player, enemies, MAX_ENEMIES, &road, showRoad);
        Render_DrawScreen(target, gamewidth, gameheight, money, lives, passengers, MAX_PASSENGERS, paused, false, player.pos, (int)levelTime);
    }

    for(int i=0;i<4;i++) UnloadTexture(car[i]);
    UnloadTexture(map[0]);
    UnloadRenderTexture(target);
    CloseWindow();

    return 0;
}
