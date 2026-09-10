#ifndef ENEMY_H
#define ENEMY_H

#include <raylib.h>
#include <stdbool.h>

// The most enemy cars we will have driving at the same time.
#define MAX_ENEMIES 6

typedef struct {
    bool active;     // false = this slot is empty, ready to be filled
    Vector2 pos;     // where the enemy car is
    float rotation;  // which way the car is pointing (used to face its texture)
    float speed;     // how fast it drives (world units per frame)
    int   waypoint;  // which waypoint in the route it is heading towards
    Texture2D texture;
    float width;     // size of the enemy car, used for drawing and collisions
    float height;
} Enemy;

// Places a car in the first empty slot at the given waypoint, heading forward
// along the route from there. The route loops forever (little round-trip).
void Enemy_Spawn(Enemy* enemies, int max, int spawnpoint, Texture2D texture);

// Moves every active enemy one step along its (looping) route.
void Enemy_Update(Enemy* enemies, int max);

// The number of waypoints in the enemy route.
int Enemy_RouteLength(void);

#endif
