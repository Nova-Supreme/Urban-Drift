#ifndef PLAYER_H
#define PLAYER_H

#include <raylib.h>
#include "cars.h"
#include "road.h"

typedef struct {
    Vector2 pos;
    float rotation;
    float targetRotation;
    float speed;
    vehicle veh;
    Texture2D texture;
} Player;

void Player_HandleInput(Player* player);
void Player_UpdatePosition(Player* player, RoadNetwork* road);

#endif
