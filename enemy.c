#include "enemy.h"
#include <math.h>

// The route the enemy cars drive along. The points are placed down the CENTRE
// of the road pieces defined in road.c, so every one of them is guaranteed to
// be on the road. Enemies simply drive from one waypoint to the next.
static const Vector2 WAYPOINTS[] = {
    { 480, 239 }, // top of the central road
    { 495, 386 }, // ...down the central road...
    { 510, 576 },
    { 511, 748 },
    { 586, 885 },
    { 507, 1018 }, // bottom of the central road
    { 1082, 1131 } // across the bottom horizontal road
};

#define WAYPOINT_COUNT (sizeof(WAYPOINTS) / sizeof(WAYPOINTS[0]))

// How close an enemy must get to a waypoint before it counts as "reaching"
// it and moves on to the next one.
#define WAYPOINT_REACH 8.0f

int Enemy_RouteLength(void)
{
    return (int)WAYPOINT_COUNT;
}

void Enemy_Spawn(Enemy* enemies, int max, int spawnpoint, Texture2D texture)
{
    // Find the first empty slot so we never run over MAX_ENEMIES cars.
    for (int i = 0; i < max; i++)
    {
        if (enemies[i].active) continue;

        // Start at the chosen waypoint, heading forward along the route.
        enemies[i].active    = true;
        enemies[i].pos       = WAYPOINTS[spawnpoint];
        enemies[i].rotation  = 0.0f;
        enemies[i].speed     = 0.7f;
        enemies[i].waypoint  = (spawnpoint + 1) % WAYPOINT_COUNT;
        enemies[i].direction = 1;
        enemies[i].texture   = texture;
        enemies[i].width     = 30.0f;
        enemies[i].height    = 50.0f;
        return;
    }
}

void Enemy_Update(Enemy* enemies, int max)
{
    for (int i = 0; i < max; i++)
    {
        if (!enemies[i].active) continue;

        // The waypoint we are currently driving towards.
        Vector2 target = WAYPOINTS[enemies[i].waypoint];

        // Direction from the car to the target.
        float dx = target.x - enemies[i].pos.x;
        float dy = target.y - enemies[i].pos.y;
        float distance = sqrtf(dx*dx + dy*dy);

        if (distance < WAYPOINT_REACH)
        {
            // We arrived. Move to the next waypoint in the current direction.
            enemies[i].waypoint += enemies[i].direction;

            // If we passed an end of the route, turn around and go back the
            // other way (this keeps the car on-road, patrolling up and down).
            if (enemies[i].waypoint < 0 || enemies[i].waypoint >= (int)WAYPOINT_COUNT)
            {
                enemies[i].direction = -enemies[i].direction;
                enemies[i].waypoint += enemies[i].direction;
            }
        }
        else
        {
            // Otherwise drive one small step towards the target.
            float step = enemies[i].speed;
            enemies[i].pos.x += (dx / distance) * step;
            enemies[i].pos.y += (dy / distance) * step;

            // Point the car in the direction it is driving.
            enemies[i].rotation = atan2f(dx, -dy) * RAD2DEG;
        }
    }
}
