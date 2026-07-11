#include <stdio.h>
#include <raylib.h>
#include <math.h>
#include "cars.h"

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
    SetTextureFilter(target.texture,TEXTURE_FILTER_BILINEAR); // no understand
    SetTargetFPS(144);

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
        float rightwall=gamewidth-playervehicle.width/2.0f; 
        if(nextX<leftwall) pos.x=leftwall;
        else if(nextX>rightwall) pos.x=rightwall;
        else pos.x=nextX;
        float topwall=playervehicle.height/2.0f;
        float bottomwall=gameheight-playervehicle.height/2.0f; 
        float nextY=pos.y+moveY;
        if(nextY<topwall) pos.y=topwall;
        else if(nextY>bottomwall) pos.y=bottomwall;
        else pos.y=nextY;

        // --- DRAWING TO CANVAS ---
        BeginTextureMode(target);
            ClearBackground(RAYWHITE);
            DrawRectanglePro(
                (Rectangle){pos.x,pos.y,playervehicle.width,playervehicle.height},
                (Vector2){playervehicle.width/2.0f,playervehicle.height/2.0f}, 
                rotation,
                BLUE);
        EndTextureMode();
        
        // --- SCALING CANVAS TO WINDOW (No distortion) ---
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
                (Rectangle){viewX,viewY,viewWidth,viewHeight}, // Centered and scaled evenly!
                (Vector2){0,0}, 
                0.0f,
                WHITE);
        EndDrawing();
    }
    UnloadRenderTexture(target);
    CloseWindow();
}