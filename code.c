#include <stdio.h>
#include <raylib.h>
#include <math.h>

int main()
{
    float rotation = 0.0f;
    float speed = 0.0f;

    float maxspeed = 3.0f;
    float acceleration = 0.01f;
    float friction = 0.005f;

    float worldWidth = 1024;
    float worldHeight = 2304;


    // Car position
    Vector2 pos = {512.0f, 500.0f};


    InitWindow(1024,768,"Urban Drift");


    Texture2D car = LoadTexture("game/gari/gari_1/gari_1_01.png");
    Texture2D background = LoadTexture("game/gari/Background_Tiles/Soil_Tile.png");


    Camera2D camera = {0};

    camera.target = pos;
    camera.offset = (Vector2){512,384};
    camera.rotation = 0.0f;
    camera.zoom = 1.5f;


    SetTargetFPS(144);


    float carWidth = 30;
    float carHeight = 50;

    float halfWidth = carWidth/2;
    float halfHeight = carHeight/2;



    while(!WindowShouldClose())
    {

        bool hitBoundary = false;



        // =====================
        //      CONTROLS
        // =====================


        if(IsKeyDown(KEY_D))
            rotation += 0.67f;


        if(IsKeyDown(KEY_A))
            rotation -= 0.67f;



        // Acceleration only if not hitting wall

        if(IsKeyDown(KEY_W) && speed < maxspeed && !hitBoundary)
        {
            speed += acceleration;
        }


        if(IsKeyDown(KEY_S) && speed > -maxspeed && !hitBoundary)
        {
            speed -= acceleration;
        }




        // =====================
        //       FRICTION
        // =====================


        if(IsKeyUp(KEY_W) && IsKeyUp(KEY_S))
        {
            if(speed > 0)
                speed -= friction;

            if(speed < 0)
                speed += friction;


            if(fabs(speed)<0.01f)
                speed=0;
        }




// =====================
//      MOVEMENT
// =====================

float moveX = sinf(rotation * DEG2RAD) * speed;
float moveY = -cosf(rotation * DEG2RAD) * speed;


// Try moving

pos.x += moveX;
pos.y += moveY;


// =====================
//     BOUNDARIES
// =====================


// Left wall
if(pos.x - halfWidth < 0)
{
    pos.x = halfWidth;
    speed = 0;
}


// Right wall
if(pos.x + halfWidth > worldWidth)
{
    pos.x = worldWidth - halfWidth;
    speed = 0;
}


// Top wall
if(pos.y - halfHeight < 0)
{
    pos.y = halfHeight;
    speed = 0;
}


// Bottom wall
if(pos.y + halfHeight > worldHeight)
{
    pos.y = worldHeight - halfHeight;
    speed = 0;
}





        // =====================
        //       CAMERA
        // =====================


        camera.target.x = 512;


        if(pos.y < 384)
            camera.target.y = 384;

        else if(pos.y > worldHeight-384)
            camera.target.y = worldHeight-384;

        else
            camera.target.y = pos.y;





        // =====================
        //        DRAW
        // =====================


        BeginDrawing();


        ClearBackground(RAYWHITE);


        BeginMode2D(camera);



        // Background tiles

        for(int y=0; y<worldHeight; y+=background.height)
        {
            DrawTexture(background,0,y,WHITE);
        }




        // Optional: show boundary

        DrawRectangleLines(
            0,
            0,
            worldWidth,
            worldHeight,
            RED
        );





        // Car sprite

        DrawTexturePro(
            car,
            (Rectangle){0,0,car.width,car.height},

            (Rectangle)
            {
                pos.x,
                pos.y,
                carWidth,
                carHeight
            },

            (Vector2)
            {
                halfWidth,
                halfHeight
            },

            rotation,

            WHITE
        );



        EndMode2D();



        EndDrawing();

    }



    UnloadTexture(background);
    UnloadTexture(car);


    CloseWindow();


    return 0;
}