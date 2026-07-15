#include "player.h"
#include <math.h>

void Player_HandleInput(Player* player)
{
    int vertical = 0;
    int horizontal = 0;

    if(IsKeyDown(KEY_W) && !IsKeyDown(KEY_S))
        vertical = -1;
    else if(IsKeyDown(KEY_S) && !IsKeyDown(KEY_W))
        vertical = 1;

    if(IsKeyDown(KEY_D) && !IsKeyDown(KEY_A))
        horizontal = 1;
    else if(IsKeyDown(KEY_A) && !IsKeyDown(KEY_D))
        horizontal = -1;

    bool isMoving = (vertical != 0 || horizontal != 0);

    if(isMoving)
    {
        if(vertical==-1 && horizontal==0)       player->targetRotation =   0.0f;
        else if(vertical==-1 && horizontal==1)  player->targetRotation =  45.0f;
        else if(vertical==0  && horizontal==1)  player->targetRotation =  90.0f;
        else if(vertical==1  && horizontal==1)  player->targetRotation = 135.0f;
        else if(vertical==1  && horizontal==0)  player->targetRotation = 180.0f;
        else if(vertical==1  && horizontal==-1) player->targetRotation = 225.0f;
        else if(vertical==0  && horizontal==-1) player->targetRotation = 270.0f;
        else if(vertical==-1 && horizontal==-1) player->targetRotation = 315.0f;
    }

    float turnSpeed = 3.0f;
    float diff = player->targetRotation - player->rotation;

    while(diff > 180.0f) diff -= 360.0f;
    while(diff < -180.0f) diff += 360.0f;

    if(fabsf(diff) <= turnSpeed)
        player->rotation = player->targetRotation;
    else
        player->rotation += (diff > 0.0f) ? turnSpeed : -turnSpeed;

    if(player->rotation < 0.0f) player->rotation += 360.0f;
    if(player->rotation >= 360.0f) player->rotation -= 360.0f;

    if(IsKeyDown(KEY_SPACE) && player->speed != 0.0f)
    {
        if(player->speed > 0.0f)
        {
            player->speed -= player->veh.brakeforce;
            if(player->speed < 0.0f) player->speed = 0.0f;
        }
    }
    else
    {
        if(isMoving && player->speed < player->veh.maxspeed)
        {
            player->speed += player->veh.acceleration;
        }
        else if(!isMoving && player->speed != 0.0f)
        {
            player->speed -= player->veh.friction;
            if(player->speed < 0.01f)
                player->speed = 0.0f;
        }
    }
}

void Player_UpdatePosition(Player* player, float worldwidth, float worldheight)
{
    float moveX = sinf(player->rotation*DEG2RAD)*player->speed;
    float moveY = -cosf(player->rotation*DEG2RAD)*player->speed;

    float nextX = player->pos.x + moveX;
    float leftwall = player->veh.width/2.0f;
    float rightwall = worldwidth - player->veh.width/2.0f;
    if(nextX < leftwall) player->pos.x = leftwall;
    else if(nextX > rightwall) player->pos.x = rightwall;
    else player->pos.x = nextX;

    float nextY = player->pos.y + moveY;
    float topwall = player->veh.height/2.0f;
    float bottomwall = worldheight - player->veh.height/2.0f;
    if(nextY < topwall) player->pos.y = topwall;
    else if(nextY > bottomwall) player->pos.y = bottomwall;
    else player->pos.y = nextY;
}
