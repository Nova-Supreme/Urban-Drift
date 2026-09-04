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

void Player_UpdatePosition(Player* player, RoadNetwork* road)
{
    // How far the car wants to move this frame, along the way it is facing.
    float moveX = sinf(player->rotation*DEG2RAD)*player->speed;
    float moveY = -cosf(player->rotation*DEG2RAD)*player->speed;

    // The car may only move if it STAYS ON the road. We try the full move
    // first; if that would leave the road, we try each axis on its own. This
    // lets the car slide along a road edge instead of sticking to it.
    if (Road_Contains(road, (Vector2){player->pos.x + moveX, player->pos.y + moveY}))
    {
        player->pos.x += moveX;
        player->pos.y += moveY;
    }
    else if (Road_Contains(road, (Vector2){player->pos.x + moveX, player->pos.y}))
    {
        player->pos.x += moveX;
    }
    else if (Road_Contains(road, (Vector2){player->pos.x, player->pos.y + moveY}))
    {
        player->pos.y += moveY;
    }
    // ponytail: on a very sharp corner the car can stop a little earlier than
    // a perfect collision would; the axis-split check is good enough for now.
}
