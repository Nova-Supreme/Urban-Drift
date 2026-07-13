#include "aicar.h"
#include <stdlib.h>
#include <math.h>

void SpawnSingleAICar(AICar *car, float worldwidth, float worldheight, float radius) {
    car->pos.x = 50.0f + (float)(rand() % (int)(worldwidth - 100.0f));
    car->pos.y = 50.0f + (float)(rand() % (int)(worldheight - 100.0f));
    car->rotation = (float)(rand() % 360);
    car->speed = 40.0f + (float)(rand() % 40); // pixels/sec, constant
    car->radius = radius;
    car->textureIndex = rand() % 4;
    car->active = true;
}

void SpawnAICars(AICar aicars[], int count, float worldwidth, float worldheight, float radius) {
    for (int i = 0; i < count; i++) {
        SpawnSingleAICar(&aicars[i], worldwidth, worldheight, radius);
    }
}

void UpdateAICars(AICar aicars[], int count, float worldwidth, float worldheight, float dt) {
    for (int i = 0; i < count; i++) {
        if (!aicars[i].active) continue;

        float aiMoveX = sinf(aicars[i].rotation*DEG2RAD)*aicars[i].speed*dt;
        float aiMoveY = -cosf(aicars[i].rotation*DEG2RAD)*aicars[i].speed*dt;
        float aiNextX = aicars[i].pos.x + aiMoveX;
        float aiNextY = aicars[i].pos.y + aiMoveY;

        if (aiNextX < aicars[i].radius || aiNextX > worldwidth - aicars[i].radius ||
            aiNextY < aicars[i].radius || aiNextY > worldheight - aicars[i].radius) {
            aicars[i].active = false; // hit the border: disappear instead of bouncing
        } else {
            aicars[i].pos.x = aiNextX;
            aicars[i].pos.y = aiNextY;
        }
    }
}

void PeriodicRespawnAICars(AICar aicars[], int count, float worldwidth, float worldheight,
                            float radius, float dt, float *timer, float interval) {
    *timer += dt;
    if (*timer >= interval) {
        *timer = 0.0f;
        for (int i = 0; i < count; i++) {
            if (!aicars[i].active) {
                SpawnSingleAICar(&aicars[i], worldwidth, worldheight, radius);
                break; // only bring back one car per interval, not all at once
            }
        }
    }
}

void DrawAICars(AICar aicars[], int count, Texture2D cartextures[], float carWidth, float carHeight) {
    for (int i = 0; i < count; i++) {
        if (!aicars[i].active) continue;
        Texture2D aitex = cartextures[aicars[i].textureIndex];
        DrawTexturePro(
            aitex,
            (Rectangle){0.0f,0.0f,(float)aitex.width,(float)aitex.height},
            (Rectangle){aicars[i].pos.x,aicars[i].pos.y,carWidth,carHeight},
            (Vector2){carWidth/2.0f,carHeight/2.0f},
            aicars[i].rotation,
            WHITE);
    }
}

bool CheckAICarCollision(AICar aicars[], int count, Vector2 playerPos, float playerRadius) {
    for (int i = 0; i < count; i++) {
        if (!aicars[i].active) continue;
        float dx = playerPos.x - aicars[i].pos.x;
        float dy = playerPos.y - aicars[i].pos.y;
        float distSq = dx*dx + dy*dy;
        float radiusSum = playerRadius + aicars[i].radius;
        if (distSq < radiusSum*radiusSum) {
            return true;
        }
    }
    return false;
}
