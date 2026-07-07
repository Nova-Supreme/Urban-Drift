#include <stdio.h>
#include <raylib.h>
#include <math.h>

int main()
{
    float rotation = 0.0f;
    float speed=0.0f;
    float maxspeed=7.0f;
    float acceleration=0.05f;
    float deceleration=0.08f;
    float friction=0.01f;

    Vector2 pos={0.0f,0.0f};
    
    InitWindow(1024,768,"Urban Drift");
    SetTargetFPS(144);
    while(!WindowShouldClose()){
        if(IsKeyDown(KEY_D)) rotation+=0.75f;
        if(IsKeyDown(KEY_A)) rotation-=0.75f;
        if(IsKeyDown(KEY_W) && speed<maxspeed && !IsKeyDown(KEY_S)) speed+=acceleration;
        if(IsKeyDown(KEY_S) && speed>-maxspeed && !IsKeyDown(KEY_W)) speed-=acceleration;
        if(IsKeyUp(KEY_W)&&IsKeyUp(KEY_S)&&speed!=0){
            if(speed>0) speed-=friction;
            if(speed<0) speed+=friction;
            if (speed<0.01f&&speed>-0.01f) speed=0.0f;
        }
        pos.x+=sinf(rotation*DEG2RAD)*speed;
        pos.y-=cosf(rotation*DEG2RAD)*speed;
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawRectanglePro(
            (Rectangle){pos.x,pos.y,50,80},
            (Vector2){25,40}, //origin point of rotation (center of the car)
            rotation,
            BLUE);
        EndDrawing();
    }
    CloseWindow();
}