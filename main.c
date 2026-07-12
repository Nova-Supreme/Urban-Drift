#include <stdio.h>
#include <raylib.h>
#include <math.h>
#include "cars.h"

int main()
{
    Vector2 pos={50.0f,50.0f};
    float rotation=0.0f;

    float speed=0.0f;
    vehicle playervehicle=GetVehiclePreset(1);
    
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

    //Texture2D mapbackground=LoadTexture("assets/samplebg2.png");

    float worldwidth=1024.0f;
    float worldheight=768.0f;

    Camera2D camera={0};
    camera.target=pos;
    camera.offset=(Vector2){(float)gamewidth/2.0f,(float)gameheight/2.0f}; 
    camera.rotation=0.0f;
    camera.zoom=1.0f;

    //SetExitKey(KEY_NULL);
    
    while(!WindowShouldClose()){
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

        if(pos.x<(float)gamewidth/2.0f) camera.target.x=(float)gamewidth/2.0f;
        else if(pos.x>worldwidth-((float)gamewidth/2.0f)) camera.target.x=worldwidth-((float)gamewidth/2.0f);
        else camera.target.x = pos.x;
        if(pos.y<(float)gameheight/2.0f) camera.target.y=(float)gameheight/2.0f;
        else if(pos.y>worldheight-((float)gameheight/2.0f)) camera.target.y=worldheight-((float)gameheight/2.0f);
        else camera.target.y = pos.y;

        // --- DRAWING TO CANVAS ---
        BeginTextureMode(target);
            ClearBackground(RAYWHITE);
            BeginMode2D(camera);

            // Rectangle mapsource={0.0f,0.0f,(float)mapbackground.width,(float)mapbackground.height};
            // Rectangle mapdest={0.0f,0.0f,worldwidth,worldheight};
            // DrawTexturePro(mapbackground,mapsource,mapdest,(Vector2){0.0f,0.0f},0.0f,WHITE);

            DrawTexturePro(
                currenttexture,
                (Rectangle){0.0f,0.0f,(float)currenttexture.width,(float)currenttexture.height},
                (Rectangle){pos.x,pos.y,playervehicle.width,playervehicle.height},
                (Vector2){playervehicle.width/2.0f,playervehicle.height/2.0f},
                rotation,
                WHITE);
                EndMode2D();
        EndTextureMode();
        
        BeginDrawing();
            ClearBackground(BLACK); //black fills the remaining space
            
            float windowwidth=GetScreenWidth();
            float windowheight=GetScreenHeight();
            
            // Calculate scale factor while preserving 1024x768 ratio
            float scale=fminf(windowwidth/(float)gamewidth,windowheight/(float)gameheight);
            float viewWidth=(float)gamewidth*scale;
            float viewHeight=(float)gameheight*scale;
            float viewX=(windowwidth-viewWidth)*0.5f;
            float viewY=(windowheight-viewHeight)*0.5f;

            DrawTexturePro(
                target.texture,
                (Rectangle){0,0,(float)target.texture.width,(float)-target.texture.height},
                (Rectangle){viewX,viewY,viewWidth,viewHeight}, // centered and scaled evenly
                (Vector2){0,0}, 
                0.0f,
                WHITE);
        EndDrawing();
    }
    for(int i=0;i<4;i++) UnloadTexture(cartextures[i]);
    UnloadRenderTexture(target);
    CloseWindow();
}