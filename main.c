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
#include "records.h"

// Which screen the game is showing right now.
typedef enum {
    STATE_INTRO,       // the opening slideshow (shown once, on launch)
    STATE_TITLE,       // the main menu
    STATE_PLAYING,     // driving round
    STATE_OVER,        // the "game over" screen
    STATE_UNLOCK,      // "(vehicle) unlocked!" alert + 5 second countdown
    STATE_ENDING,      // the ending cutscene (ending.gif, played once, after the CAR)
    STATE_NAMEINPUT   // typing a name for the high score (after the CAR)
} GameState;

// The map image is stretched onto a rectangle of this size. map1 is made
// LARGER than map0 so its roads render thicker against the car's fixed 30px
// size (at map0's scale they felt too thin). Raise these to stretch further.
#define WORLD_W0 2048.0f
#define WORLD_H0 1536.0f
#define WORLD_W1 3072.0f
#define WORLD_H1 2304.0f
#define WORLD_W2 1536.0f
#define WORLD_H2 1152.0f

// The ending cutscene was split from assets/ending.gif into one PNG per
// frame (assets/ending/f0000.png ...). ENDING_FRAME_TIME is the gif's average
// frame delay, so it replays at the speed it was made with.
#define ENDING_FRAMES 104
#define ENDING_FRAME_TIME 0.06f

// Switches the player's car to `vehicleId` and swaps its picture.
static void SetVehicle(Player* player, Texture2D* carTextures, int vehicleId)
{
    player->veh = GetVehiclePreset(vehicleId);
    player->texture = carTextures[vehicleId - 1];
}

// Puts the current level back to its very start: the car at spawn, full lives,
// all passengers and enemies cleared, fare multiplier back to 1.0.
// NOTE: money is NOT reset here - it is a running total that survives from one
// level/run to the next, and it is exactly what unlocks the vehicles. The
// money multiplier resets because a brand-new level starts your fares at $100.
static void ResetRun(Player* player, int* lives, int* score,
                     float* hurtTimer, Enemy* enemies,
                     Passenger* passengers, int passengerCount,
                     Intro* intro, float* moneyMultiplier, int level)
{
    // Spawn the car ON the level's own road, pointing along it. map0 spawns at
    // the west end of its central circle road; map2 and map1 both start on an
    // upper-right stretch of road, facing left down it.
    if(level == 0){     // map2 (the first map, smallest world)
        player->pos = (Vector2){1250.0f, 350.0f};
        player->rotation = 270.0f; // facing left (west)
        player->targetRotation = 270.0f;
    }
    else if(level == 1){ // map0 (the tutorial map)
        player->pos = (Vector2){170.0f, 350.0f};
        player->rotation = 90.0f;
        player->targetRotation = 90.0f;
    }
    else{               // map1 (the hand-drawn town map)
        player->pos = (Vector2){2500.0f, 700.0f};
        player->rotation = 270.0f; // facing left (west)
        player->targetRotation = 270.0f;
    }
    player->speed = 0.0f;

    *lives = 5;
    *score = 0;
    *hurtTimer = 0.0f;
    *moneyMultiplier = 1.0f;

    // Empty all enemy slots - StartTraffic() fills in the level's cars right
    // after this runs.
    for(int i=0;i<MAX_ENEMIES;i++) enemies[i].active = false;

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

// Stores the world (map-image stretch) dimensions for the given level. Each
// level switch must call this before drawing so the map fills its own size.
// Map2 (the first map, level 0) is the smallest world; map1 (level 2) is the
// biggest.
static void SetWorldSize(int level, float* worldwidth, float* worldheight)
{
    if(level == 0){ *worldwidth = WORLD_W2; *worldheight = WORLD_H2; }
    else if(level == 1){ *worldwidth = WORLD_W0; *worldheight = WORLD_H0; }
    else          { *worldwidth = WORLD_W1; *worldheight = WORLD_H1; }
}

// Places the cars that are driving when the level starts. There is NO random
// spawning - every vehicle is already sitting on its route when the player
// appears, and just follows its path. This is where future levels would add
// their own cars.
static void StartTraffic(Enemy* enemies, Texture2D* carTextures, int level)
{
    // Tutorial map (map0 = level 1): exactly ONE car on the long horizontal
    // road. It starts at waypoint 0 (the west end) and loops around the road.
    // Routes are hand-tuned per map in enemy.c - map2 and map1 have no
    // traffic yet, so it only starts cars when level == 1.
    if(level == 1)
        Enemy_Spawn(enemies, MAX_ENEMIES, 0, carTextures[2]);
}

// Sets up the bits of state that start fresh with each level.
static void PrepareLevel(float* passengerTimer)
{
    *passengerTimer = 2.0f; // how long before the first passenger appears
}

// Saves the player's progress to disk. `active` says whether the round is
// still going (1) or was lost (0); either way the money is kept. `level` is
// which map the round is on, so CONTINUE can pick up at the same spot.
static void SaveProgress(int money, int vehicleId, int ownedMask, int completedMask, int active, int level)
{
    // Build the struct field-by-field (named assignment) so a value can never
    // slip into the wrong slot and corrupt the saved progress.
    SaveData d;
    d.money          = money;
    d.savedVehicleId = vehicleId;
    d.ownedMask      = ownedMask;
    d.active         = active;
    d.completedMask  = completedMask;
    d.level          = level;
    Save_Write(d, active);
}

// Called when a level starts (fresh run or retry): resets the whole world and
// jumps into the driving state.
// NOTE: the caller already points `road` at this level's road layout before
// calling us (road = &roadNets[level]) - this helper only resets the world.
static void BeginLevel(Player* player, int* lives, int* score, float* hurtTimer,
                       Enemy* enemies, Passenger* passengers, int passengerCount,
                       Intro* intro, float* moneyMultiplier, Texture2D* car,
                       int level, float* passengerTimer, float* worldwidth,
                       float* worldheight, bool* paused)
{
    SetWorldSize(level, worldwidth, worldheight);
    ResetRun(player, lives, score, hurtTimer, enemies, passengers, passengerCount,
             intro, moneyMultiplier, level);
    StartTraffic(enemies, car, level);
    PrepareLevel(passengerTimer);
    *paused = false;
}

// Starts a run: fresh games begin on map2 in the rickshaw with a reset clock;
// CONTINUE restarts the level that was being played when the game was quit.
// The menu picks the map (level 0 = map2, 1 = map0, 2 = map1).
// Returns the level that was started, so the caller can point the road and
// camera at the right map.
static int StartRun(Player* player, Texture2D* car, int level, int vehicleId,
                    bool freshRun, float* overallTime, int* lives, int* score,
                    float* hurtTimer, Enemy* enemies, Passenger* passengers,
                    int passengerCount, Intro* intro, float* moneyMultiplier,
                    float* passengerTimer, float* worldwidth, float* worldheight,
                    bool* paused)
{
    if(level < 0) level = 0;
    if(level > 2) level = 2;
    if(vehicleId < 1 || vehicleId > 4) vehicleId = 1;
    SetVehicle(player, car, vehicleId);
    if(freshRun) *overallTime = 0.0f; // a fresh run always times from zero
    BeginLevel(player, lives, score, hurtTimer, enemies, passengers, passengerCount,
               intro, moneyMultiplier, car, level, passengerTimer,
               worldwidth, worldheight, paused);
    return level;
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

    Texture2D map[3];
    map[0]=LoadTexture("assets/map2.png");
    map[1]=LoadTexture("assets/map0.png");
    map[2]=LoadTexture("assets/map1.png");

    // ---- The ending cutscene (ending.gif, split into one PNG per frame) ----
    // Plays exactly once, right after the CAR unlock alert, then the player
    // types their name. The frames live in assets/ending/ as f0000.png...
    Texture2D endingFrames[ENDING_FRAMES];
    char framePath[64];
    for(int i = 0; i < ENDING_FRAMES; i++){
        snprintf(framePath, sizeof framePath, "assets/ending/f%04d.png", i);
        endingFrames[i] = LoadTexture(framePath);
    }
    int endingFrame = 0;    // which ending.png frame is on screen
    float endingTimer = 0.0f; // seconds on the current frame

    // ---- The player's car (starts as the rickshaw) ----
    Player player = {0};
    SetVehicle(&player, car, 1);

    // ---- The world (map, camera, road, enemies) ----
    float worldwidth;
    float worldheight;
    SetWorldSize(0, &worldwidth, &worldheight); // map2's size before any level is chosen

    Camera2D camera={0};
    camera.target=player.pos;
    camera.offset=(Vector2){(float)gamewidth/2.0f,(float)gameheight/2.0f};
    camera.rotation=0.0f;
    camera.zoom=1.0f;

    // One road layout per level; `road` points at the current level's one.
    RoadNetwork roadNets[3];
    Road_LoadMap2(&roadNets[0]);
    Road_Load(&roadNets[1]);
    Road_LoadMap1(&roadNets[2]);
    RoadNetwork* road = &roadNets[0];
    int level = 0; // which map we are on: 0 = map2 (first), 1 = map0, 2 = map1

    Enemy enemies[MAX_ENEMIES] = {0};

    // ---- The passengers for the round ----
    // Several can be on the road (and carried) at once, so it's an array.
    Passenger passengers[MAX_PASSENGERS] = {0};

    // ---- Run state (score, lives, hurt window) ----
    // money persists between runs (from the save file). You never spend it -
    // the money total is what unlocks the next vehicle when it reaches its
    // purchase price. ownedMask/completedMask are written for save-file
    // compatibility but no longer change how the game plays.
    int money          = 0;
    int ownedMask      = 1;
    int savedVehicleId = 1;
    int completedMask  = 0;
    int score          = 0;
    int lives          = 5;
    const float hurtTime=1.0f;
    float hurtTimer=0.0f;
    bool paused=false;

    // The fare multiplier for the current level: starts at 1.0 and climbs 0.1
    // with every passenger delivered (see Passenger_Update). ResetRun sets it
    // back to 1.0 for each new level.
    float moneyMultiplier = 1.0f;

    // The overall run clock. It keeps counting across ALL the levels and
    // retries of a run, and only stops when the CAR is unlocked - that total
    // time goes into the world record tracker with the name the player picks.
    float overallTime = 0.0f;

    // ---- The unlock alert ----
    // When money reaches the next vehicle's price the game stops, shows the
    // "(vehicle) unlocked!" alert for a few seconds, then moves to the next
    // level with the new vehicle. These hold the alert's state.
    int   unlockedVehicle = 0;  // the vehicle that was just unlocked (2..4)
    float unlockTimer = 5.0f;   // how long the alert stays up

    // ---- The world record name input ---- 
    char  nameInput[RECORD_NAME_MAX + 1] = {0};
    int   nameLen = 0;

    // The world-record tracker (a separate file on purpose, so it survives NEW
    // GAME resets and is only ever added to by finishing the CAR run).
    RecordEntry records[MAX_RECORDS];
    int recordCount = Records_Load(records, MAX_RECORDS);

    // Timer before the next passenger appears (they keep coming one after
    // another while you drive).
    float passengerTimer=0.0f;

    // Pull the saved money into memory if a save already exists.
    SaveData saved;
    if(Save_Load(&saved)){
        money          = saved.money;
        ownedMask      = saved.ownedMask;
        savedVehicleId = saved.savedVehicleId;
        completedMask  = saved.completedMask;
        level          = saved.level; // resume whichever map was being played
    }

    // ---- Screen flow ----
    Intro intro;
    intro.over = false;
    Intro_Load(&intro);        // prepare the opening slideshow
    TitleScreen title;
    Title_Init(&title, saved.active == 1);
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
        // Re-read the save file each time the menu is shown so CONTINUE (and
        // the money/car/map it resumes) always match what is on disk.
        if(state == STATE_TITLE){
            Save_Load(&saved); // keep `saved` in step with disk for CONTINUE
            Title_Refresh(&title, Save_Exists() && saved.active == 1);
            // Pick what the player chose, if anything.
            int choice = Title_HandleMenu(&title);
            if(choice != -1){
                if(choice == TITLE_CONTINUE){
                    // Carry on the last run: same money, same car, same map.
                    // The overall clock keeps counting from where it was within
                    // this session (the timer is not stored in the save file,
                    // so a fresh app start re-times the run from zero).
                    level = StartRun(&player, car, saved.level, savedVehicleId, false,
                                     &overallTime, &lives, &score, &hurtTimer,
                                     enemies, passengers, MAX_PASSENGERS, &intro,
                                     &moneyMultiplier, &passengerTimer,
                                     &worldwidth, &worldheight, &paused);
                    road = &roadNets[level];
                    state = STATE_PLAYING;
                }
                else if(choice == TITLE_PLAY){
                    // A completely fresh game: money, cars owned and the map all
                    // start over, and the run clock resets too. Any old save is
                    // discarded, so CONTINUE disappears until this run is saved.
                    Save_Delete();
                    money = 0;
                    ownedMask = 1;
                    savedVehicleId = 1;
                    completedMask = 0;
                    level = StartRun(&player, car, 0, 1, true,
                                     &overallTime, &lives, &score, &hurtTimer,
                                     enemies, passengers, MAX_PASSENGERS, &intro,
                                     &moneyMultiplier, &passengerTimer,
                                     &worldwidth, &worldheight, &paused);
                    road = &roadNets[level];
                    state = STATE_PLAYING;
                }
                else if(choice == TITLE_QUIT){
                    quit = true;
                }
            }

            BeginDrawing();
            ClearBackground(BLACK);
            Title_DrawMenu(&title, GetScreenWidth(), GetScreenHeight(), records, recordCount);
            EndDrawing();
            continue;
        }

        // ---------------- GAME OVER SCREEN ----------------
        if(state == STATE_OVER){
            if(IsKeyPressed(KEY_R)){
                // Restart the CURRENT level (the one that was just lost). The
                // run's money and overall clock keep counting.
                BeginLevel(&player, &lives, &score, &hurtTimer,
                           enemies, passengers, MAX_PASSENGERS, &intro,
                               &moneyMultiplier, car, level, &passengerTimer,
                               &worldwidth, &worldheight, &paused);
                state = STATE_PLAYING;
            }
            if(IsKeyPressed(KEY_ESCAPE)) state = STATE_TITLE; // back to menu

            Render_DrawWorld(target, camera, map[level], worldwidth, worldheight, passengers, MAX_PASSENGERS, player, enemies, MAX_ENEMIES, road, showRoad);
            Render_DrawScreen(target, gamewidth, gameheight, money, lives, passengers, MAX_PASSENGERS, false, true, player.pos, (int)overallTime, moneyMultiplier);
            continue;
        }

        // ---------------- UNLOCK ALERT ----------------
        // Shown when money reaches the next vehicle's price. The world freezes
        // and a countdown plays: "(vehicle) unlocked! proceeding to next level".
        if(state == STATE_UNLOCK){
            if(IsKeyPressed(KEY_ESCAPE)){
                // Back out of the unlock: the new car is NOT taken yet, so the
                // save keeps the current car and level. CONTINUE resumes where
                // the alert left off, and it triggers again on the next frame.
                SaveProgress(money, player.veh.id, ownedMask, completedMask, 1, level);
                nameLen = 0;
                state = STATE_TITLE;
            }

            unlockTimer -= GetFrameTime();
            if(unlockTimer <= 0.0f){
                // Put the new vehicle in the player's hands and move to the
                // next map. Its price comes out of the money, which is what
                // made the unlock possible in the first place. Unlocking the
                // CAR (id 4) is the end of the run: it plays the ending
                // cutscene once, then the player types a name for the record.
                SetVehicle(&player, car, unlockedVehicle);
                money -= GetVehiclePreset(unlockedVehicle).cost; // pay for the new ride
                if(unlockedVehicle == 4){
                    endingFrame = 0;
                    endingTimer = 0.0f;
                    state = STATE_ENDING;
                }
                else{
                    level = (level + 1) % 3; // next map, with the new vehicle
                    road = &roadNets[level]; // the next map has its own roads
                    BeginLevel(&player, &lives, &score, &hurtTimer,
                               enemies, passengers, MAX_PASSENGERS, &intro,
                               &moneyMultiplier, car, level, &passengerTimer,
                               &worldwidth, &worldheight, &paused);
                    state = STATE_PLAYING;
                }
            }

            // The alert screen sits on a solid black background (not a
            // translucent box over the game world) so the transition to the
            // next level doesn't flash.
            BeginDrawing();
            ClearBackground(BLACK);
            const char* vehName = Title_VehicleName(unlockedVehicle);
            const char* heading = TextFormat("%s UNLOCKED!", vehName);
            const char* sub = (unlockedVehicle == 4)
                ? "run complete - cutscene next"
                : "proceeding to next level";
            const char* count = TextFormat("in %.0f...", unlockTimer);
            int w = GetScreenWidth();
            DrawText(heading, (w - MeasureText(heading, 60)) / 2, GetScreenHeight()/2 - 80, 60, YELLOW);
            DrawText(sub, (w - MeasureText(sub, 30)) / 2, GetScreenHeight()/2 + 10, 30, WHITE);
            DrawText(count, (w - MeasureText(count, 30)) / 2, GetScreenHeight()/2 + 60, 30, LIGHTGRAY);
            DrawText("ESC = save and go to menu", (w - MeasureText("ESC = save and go to menu", 18)) / 2, GetScreenHeight() - 50, 18, DARKGRAY);
            EndDrawing();
            continue;
        }

        // ---------------- ENDING CUTSCENE ----------------
        // Plays ending.gif exactly once, straight after the CAR unlock alert.
        // ESC skips it. When it finishes the player goes to the name form.
        if(state == STATE_ENDING){
            endingTimer += GetFrameTime();
            while(endingTimer >= ENDING_FRAME_TIME && endingFrame < ENDING_FRAMES - 1){
                endingTimer -= ENDING_FRAME_TIME;
                endingFrame++;
            }

            bool finished = (endingFrame == ENDING_FRAMES - 1 && endingTimer >= ENDING_FRAME_TIME);
            if(IsKeyPressed(KEY_ESCAPE)) finished = true;
            if(finished){
                nameLen = 0;
                state = STATE_NAMEINPUT;
                continue;
            }

            BeginDrawing();
            ClearBackground(BLACK);
            float screenW = (float)GetScreenWidth();
            float screenH = (float)GetScreenHeight();
            float scale = fminf(screenW / endingFrames[0].width,
                                screenH / endingFrames[0].height);
            float dw = endingFrames[0].width * scale;
            float dh = endingFrames[0].height * scale;
            DrawTexturePro(endingFrames[endingFrame],
                           (Rectangle){ 0, 0, endingFrames[0].width, endingFrames[0].height },
                           (Rectangle){ (screenW - dw)/2.0f, (screenH - dh)/2.0f, dw, dh },
                           (Vector2){ 0, 0 }, 0.0f, WHITE);
            DrawText("ESC = skip", 20, GetScreenHeight() - 30, 18, DARKGRAY);
            EndDrawing();
            continue;
        }

        // ---------------- HIGH SCORE NAME INPUT ----------------

        // Appears right after the ending cutscene. Type a name and press
        // ENTER to save it (with the total run time) to the high-score list.
        if(state == STATE_NAMEINPUT){
            // Letters, digits and backspace.
            for(int k = KEY_A; k <= KEY_Z; k++){
                if(IsKeyPressed(k) && nameLen < RECORD_NAME_MAX){
                    nameInput[nameLen++] = (char)('A' + (k - KEY_A));
                    nameInput[nameLen] = '\0';
                }
            }
            for(int k = KEY_ZERO; k <= KEY_NINE; k++){
                if(IsKeyPressed(k) && nameLen < RECORD_NAME_MAX){
                    nameInput[nameLen++] = (char)('0' + (k - KEY_ZERO));
                    nameInput[nameLen] = '\0';
                }
            }
            if(IsKeyPressed(KEY_BACKSPACE) && nameLen > 0){
                nameInput[--nameLen] = '\0';
            }

            if(IsKeyPressed(KEY_ENTER)){
                if(nameLen > 0)
                    Records_Add(records, &recordCount, MAX_RECORDS, nameInput, (int)overallTime);
                // The run is complete: save the finished progress and go back
                // to the menu. The overall clock has stopped (it only ticks
                // while driving), so the recorded time is the full run.
                SaveProgress(money, player.veh.id, ownedMask, completedMask, 0, level);
                state = STATE_TITLE;
                continue;
            }
            // Solid black background with just the name form, same as the
            // unlock alert.
            BeginDrawing();
            ClearBackground(BLACK);
            if(IsKeyPressed(KEY_ESCAPE)){
                // Skip recording; the run still ends here.
                SaveProgress(money, player.veh.id, ownedMask, completedMask, 0, level);
                state = STATE_TITLE;
                continue;
            }
            const char* unlockedTitle = "CAR UNLOCKED!";
            const char* prompt = "Enter your name:";
            int w = GetScreenWidth();
            DrawText(unlockedTitle, (w - MeasureText(unlockedTitle, 50)) / 2, GetScreenHeight()/2 - 110, 50, GOLD);
            DrawText(prompt, (w - MeasureText(prompt, 25)) / 2, GetScreenHeight()/2 - 30, 25, WHITE);
            DrawText(nameInput, (w - MeasureText(nameInput, 25)) / 2, GetScreenHeight()/2 + 10, 25, YELLOW);
            DrawText("ENTER = save record    |    ESC = skip", (w - MeasureText("ENTER = save record    |    ESC = skip", 18)) / 2, GetScreenHeight()/2 + 70, 18, DARKGRAY);
            EndDrawing();
            continue;
        }

        // ---------------- PLAYING SCREEN ----------------
        // ESC toggles the pause overlay; M on the pause screen returns to menu.
        if(IsKeyPressed(KEY_ESCAPE)) paused = !paused;
        if(paused && IsKeyPressed(KEY_M)) {
            savedVehicleId = player.veh.id;             // remember the car being driven
            SaveProgress(money, savedVehicleId, ownedMask, completedMask, 1, level); // still going
            paused = false;
            state = STATE_TITLE;
        }

        // While paused, the whole world freezes: no input, no movement, no
        // enemies, no crashing, nothing. Only the pause/quit keys above work.
        if(!paused){
            // The overall run clock only ticks while actually driving; pauses,
            // the unlock alert and the name screen don't count against it.
            overallTime += GetFrameTime();

            Player_HandleInput(&player);
            if(IsKeyPressed(KEY_F3)) showRoad = !showRoad;
            Player_UpdatePosition(&player, road);
            // A passenger is picked up (up to the car's maxpassengers) or
            // dropped off at its destination each frame.
            Passenger_Update(passengers, MAX_PASSENGERS, player.pos, player.veh.width, player.speed, &money, &moneyMultiplier, player.veh.maxpassengers);

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
                            Passenger_SpawnRandom(&passengers[i], road, worldwidth, worldheight);
                            break; // only one new passenger per tick
                        }
                    }
                    passengerTimer = 4.0f; // next one comes a few seconds later
                }
            }

            // Move every enemy car along its (looping) route.
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

            // When money reaches the NEXT vehicle's price, the level is done:
            // stop and announce the unlock. The CAR (id 4) is the last one, so
            // the run then ends and the player records their name + time.
            if(player.veh.id < 4 && money >= GetVehiclePreset(player.veh.id + 1).cost){
                unlockedVehicle = player.veh.id + 1;
                unlockTimer = 5.0f;
                SaveProgress(money, player.veh.id, ownedMask, completedMask, 1, level);
                state = STATE_UNLOCK;
            }

            // When lives hit zero the round is over. Progress is saved so the
            // player keeps the money they earned.
            if(lives <= 0){
                savedVehicleId = player.veh.id;             // remember the car being driven
                SaveProgress(money, savedVehicleId, ownedMask, completedMask, 0, level); // active = 0 (dead)
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

        Render_DrawWorld(target, camera, map[level], worldwidth, worldheight, passengers, MAX_PASSENGERS, player, enemies, MAX_ENEMIES, road, showRoad);
        Render_DrawScreen(target, gamewidth, gameheight, money, lives, passengers, MAX_PASSENGERS, paused, false, player.pos, (int)overallTime, moneyMultiplier);
    }

    // Leaving the program always saves the money so deliveries made but not
    // yet "banked" (no level finished, no death) don't get lost.
    SaveProgress(money, player.veh.id, ownedMask, completedMask, 1, level);

    for(int i=0;i<4;i++) UnloadTexture(car[i]);
    for(int i=0;i<3;i++) UnloadTexture(map[i]);
    for(int i=0;i<ENDING_FRAMES;i++) UnloadTexture(endingFrames[i]);
    UnloadRenderTexture(target);
    CloseWindow();

    return 0;
}