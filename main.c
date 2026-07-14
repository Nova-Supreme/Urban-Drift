#include <stdio.h>
#include <raylib.h>
#include <math.h>
#include "cars.h"

int main()
{
    Vector2 pos={170.0f,350.0f};
    float rotation=90.0f;

    float speed=0.0f;
    vehicle playervehicle=GetVehiclePreset(1);
    
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    int gamewidth=1024;
    int gameheight=768;
    InitWindow(gamewidth,gameheight,"Urban Drift");
    RenderTexture2D target=LoadRenderTexture(gamewidth,gameheight); //creates virtual hidden canvas
    SetTextureFilter(target.texture,TEXTURE_FILTER_BILINEAR); //when zooming in, pixels are blended smoothly together
    SetTargetFPS(144);

    Texture2D backstory[9];
    backstory[0]=LoadTexture("assets/story0.png");
    backstory[1]=LoadTexture("assets/story1.png");
    backstory[2]=LoadTexture("assets/story2.png");
    backstory[3]=LoadTexture("assets/story3.png");
    backstory[4]=LoadTexture("assets/story4.png");
    backstory[5]=LoadTexture("assets/story5.png");
    backstory[6]=LoadTexture("assets/story6.png");
    backstory[7]=LoadTexture("assets/story7.png");
    backstory[8]=LoadTexture("assets/story8.png");

    const char* backstorytext[9]={
        "He started with nothing but a rickshaw and a dream.",
        "But for that, he needed money...\n...lots of it.",
        "His ultimate desire was to have a sweet ride of his own",
        "The dream may one day be realized:\ncruising down the beach with Handsome Squidward by his side.",
        "A fateful encounter begins with a simple request,\nwith many more such encounters to come.",
        "Channelling the speed of a legend, he takes off",
        "Moving at impossible speeds, the streets ignite,\nleaving onlookers to whisper, \"Wallahi\".",
        "They arrive at the destination, leaving the passenger completely frozen.",
        "The fare is secured, and a relentless hunger awakens: \"I need more!\""
    };

    int backstoryindex=0;
    bool isbackstoryover=false;
    int totalbackstoryslides=9;

    Texture2D car[4];
    car[0]=LoadTexture("assets/rickshaw.png");
    car[1]=LoadTexture("assets/cng.png");
    car[2]=LoadTexture("assets/auto.png");
    car[3]=LoadTexture("assets/car.png");
    Texture2D cartexture=car[playervehicle.id-1];

    Texture2D map[5];
    map[0]=LoadTexture("assets/map0.png");
    //map[1]=LoadTexture("assets/samplebg1.png");
    //map[2]=LoadTexture("assets/samplebg2.png");
    //map[3]=LoadTexture("assets/samplebg3.png");
    //map[4]=LoadTexture("assets/samplebg4.png");
    Texture2D maptexture=map[0];

    float worldwidth=1024.0f*2.0f;
    float worldheight=768.0f*2.0f;

    Camera2D camera={0};
    camera.target=pos;
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
        if(!isbackstoryover){
            if(IsKeyPressed(KEY_ESCAPE)){
                isbackstoryover=true;
                for(int i=0;i<9;i++) UnloadTexture(backstory[i]);
            }
            if(IsKeyPressed(KEY_SPACE)){
                backstoryindex++;
                if(backstoryindex>=totalbackstoryslides){
                    isbackstoryover=true;
                    for(int i=0;i<9;i++) UnloadTexture(backstory[i]);
                }
            }
            BeginDrawing();
            ClearBackground(BLACK);
            if(!isbackstoryover){
                float windowwidth=GetScreenWidth();
                float windowheight=GetScreenHeight();
                Texture2D currentslide=backstory[backstoryindex];
                float imageregionheight=windowheight*0.8f;
                float scale=fminf(windowwidth/(float)currentslide.width,imageregionheight/(float)currentslide.height);
                float imgw=currentslide.width*scale;
                float imgh=currentslide.height*scale;
                float imgx=(windowwidth-imgw)/2.0f;
                float imgy=20.0f;
                DrawTexturePro(
                    currentslide,
                    (Rectangle){0,0,(float)currentslide.width,(float)currentslide.height},
                    (Rectangle){imgx,imgy,imgw,imgh},
                    (Vector2){0,0},
                    0.0f,
                    WHITE);

                int textwidth = MeasureText(backstorytext[backstoryindex],20);
                DrawText(backstorytext[backstoryindex],(windowwidth-textwidth)/2,windowheight*0.88f,20,WHITE);
                DrawText("Press SPACE to continue or ESC to skip", 20, windowheight-30, 16, GRAY);
            }
            EndDrawing();
            continue; //skip the rest of the loop entirely while backstory is showing
        }

        if(IsKeyPressed(KEY_ESCAPE)) paused=!paused;
        if(!paused){
            if (speed!=0.0f){
                if(IsKeyDown(KEY_D)) rotation+=0.5f;
                if(IsKeyDown(KEY_A)) rotation-=0.5f;
                rotation=fmodf(rotation,360.0f);
            }
            if(IsKeyDown(KEY_SPACE) && speed!=0.0f){ //emergency brake
                if(speed>0.0f){
                    speed-=playervehicle.brakeforce;
                    if(speed<0.0f) speed=0.0f;
                }
                else if(speed<0.0f){
                    speed+=playervehicle.brakeforce;
                    if(speed>0.0f) speed=0.0f;
                }
            }
            else{
                if(IsKeyDown(KEY_W) && speed<playervehicle.maxspeed && !IsKeyDown(KEY_S)) speed+=playervehicle.acceleration;
                if(IsKeyDown(KEY_S) && speed>-playervehicle.maxspeed && !IsKeyDown(KEY_W)) speed-=playervehicle.acceleration;
                if(IsKeyUp(KEY_W)&&IsKeyUp(KEY_S)&&speed!=0){
                    if(speed>0) speed-=playervehicle.friction;
                    if(speed<0) speed+=playervehicle.friction;
                    if(speed<0.01f&&speed>-0.01f) speed=0.0f;
                }
            }
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

        if(pos.x<(float)gamewidth/2.0f) camera.target.x=(float)gamewidth/2.0f;
        else if(pos.x>worldwidth-((float)gamewidth/2.0f)) camera.target.x=worldwidth-((float)gamewidth/2.0f);
        else camera.target.x=pos.x;
        if(pos.y<(float)gameheight/2.0f) camera.target.y=(float)gameheight/2.0f;
        else if(pos.y>worldheight-((float)gameheight/2.0f)) camera.target.y=worldheight-((float)gameheight/2.0f);
        else camera.target.y=pos.y;
    

        BeginTextureMode(target); //starts drawing on hidden canvas
            ClearBackground(RAYWHITE); //clears old frame and makes it white
            BeginMode2D(camera); //activates camera system

            DrawTexturePro( //draws the map onto hidden canvas
                    maptexture,
                    (Rectangle){0.0f,0.0f,(float)maptexture.width,(float)maptexture.height}, //selects area of loaded map image
                    (Rectangle){0.0f,0.0f,worldwidth,worldheight}, //stretches image across map world
                    (Vector2){0.0f,0.0f}, //origin of rotation
                    0.0f, //rotation
                    WHITE); //tint

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

            DrawTexturePro( //draws vehicle onto hidden canvas
                cartexture,
                (Rectangle){0.0f,0.0f,(float)cartexture.width,(float)cartexture.height}, //selects area of vehicle image
                (Rectangle){pos.x,pos.y,playervehicle.width,playervehicle.height}, //stretches image across vehicle size
                (Vector2){playervehicle.width/2.0f,playervehicle.height/2.0f}, //origin point of rotation 
                rotation,
                WHITE);
            EndMode2D(); //ends camera movement
        EndTextureMode(); //ends drawing on hidden canvas

        BeginDrawing(); //now starts drawing on display
            ClearBackground(BLACK); //black fills the remaining space            
            float windowwidth=GetScreenWidth();
            float windowheight=GetScreenHeight();
            
            // Calculate scale factor while preserving 1024x768 ratio
            float scale=fminf(windowwidth/(float)gamewidth,windowheight/(float)gameheight);
            float viewwidth=(float)gamewidth*scale;
            float viewheight=(float)gameheight*scale;
            float viewX=(windowwidth-viewwidth)/2.0f; //to offset the game so its in center of the screen
            float viewY=(windowheight-viewheight)/2.0f;

            DrawTexturePro( //prints virtual canvas onto monitor
                target.texture,
                (Rectangle){0,0,(float)target.texture.width,(float)-target.texture.height},
                //negative height bc raylib draws from topleft but opengl draws graphics from bottomleft
                (Rectangle){viewX,viewY,viewwidth,viewheight}, //centered and scaled evenly
                (Vector2){0,0}, //rotation point
                0.0f, //rotation angle
                WHITE);

            DrawText(TextFormat("MONEY:%04d",money),20,20,30,BLUE);
            if(currentpassenger.isspawned && currentpassenger.ispickedup) DrawText("DESTINATION MARKED ON MAP",20,60,20,GOLD);
            
            if(paused){
                DrawRectangle(0,0,windowwidth,windowheight,(Color){0,0,0,150}); //semi transparent black overlay
                const char* pausedtext="PAUSED";
                int textwidth=MeasureText(pausedtext,50); //calculates width in pixels based on fontsize
                DrawText(pausedtext,(windowwidth-textwidth)/2,windowheight/2-25,50,WHITE);
            }

        EndDrawing(); //stops drawing on screen
    }
    for(int i=0;i<4;i++) UnloadTexture(car[i]);
    //for(int i=0;i<5;i++) UnloadTexture(map[i]);
    UnloadTexture(map[0]);
    UnloadRenderTexture(target);
    CloseWindow();
}