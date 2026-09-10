#include "enemy.h"
#include "cars.h"
#include <math.h>

// The route the tutorial car drives: a smooth rectangle loop around the long
// horizontal road (the bottom road across map0), keeping to the LEFT-hand
// lanes. Going right (east) is the TOP lane (kept a touch higher), going left
// (west) is the BOTTOM lane. Each end of the road is a curved U-turn so the
// car turns smoothly instead of snapping at a right angle.
// The U-turn waypoints are spaced ~9px apart (~22.5 degrees of arc at the
// east end, 30 at the west) - at the car's 1.2px/frame speed that lets the
// eased turning keep up, so the curve comes out round.
// NOTE: values tuned by hand - adjust the lane y's freely.
static const Vector2 WAYPOINTS[] = {
    {  520, 1095 }, // 0: west end, top lane (eastbound) - drive east
    { 1693.5, 1095 }, // 1: east end of the top lane
    { 1702.1, 1096.7 }, // 2: ease off the top lane (east U-turn start)
    { 1709.4, 1101.6 }, // 3
    { 1714.3, 1108.9 }, // 4
    { 1716.0, 1117.5 }, // 5: middle of the east U-turn (out to the road edge)
    { 1714.3, 1126.1 }, // 6
    { 1709.4, 1133.4 }, // 7
    { 1702.1, 1138.3 }, // 8: ease onto the bottom lane
    { 1693.5, 1140 }, // 9: east end of the bottom lane
    {  504, 1140 },  // 10: west end of the bottom lane - drive west
    {  495.5, 1137.7 }, // 11: (west U-turn start)
    {  489.3, 1131.5 }, // 12
    {  487, 1123 },  // 13: middle of the west U-turn (out to the road edge)
    {  489.3, 1114.5 }, // 14
    {  495.5, 1108.3 }, // 15: (west U-turn end)
    {  504, 1106 }   // 16: back on the top lane, west end (merges onto waypoint 0)
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
        // EVERY enemy drives at the rickshaw's constant top speed, so traffic
        // never catches the player - you can keep up but still must steer.
        enemies[i].speed     = GetVehiclePreset(1).maxspeed;
        enemies[i].waypoint  = (spawnpoint + 1) % WAYPOINT_COUNT;
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
            // We arrived. Move to the next waypoint; the route wraps around at
            // the end so the car loops forever (waypoint 3 -> waypoint 0).
            enemies[i].waypoint = (enemies[i].waypoint + 1) % WAYPOINT_COUNT;
        }
        else
        {
            // Otherwise drive one small step towards the target.
            float step = enemies[i].speed;
            enemies[i].pos.x += (dx / distance) * step;
            enemies[i].pos.y += (dy / distance) * step;

            // Which way the car SHOULD be pointing (towards the next waypoint).
            float targetRotation = atan2f(dx, -dy) * RAD2DEG;

            // Ease the car's facing towards that angle rather than snapping,
            // so corners look like a smooth turn, not an instant right angle.
            float turnDiff = targetRotation - enemies[i].rotation;
            while (turnDiff >  180.0f) turnDiff -= 360.0f;
            while (turnDiff < -180.0f) turnDiff += 360.0f;

            float turnSpeed = 4.0f; // degrees per frame
            if      (turnDiff >  turnSpeed) turnDiff =  turnSpeed;
            else if (turnDiff < -turnSpeed) turnDiff = -turnSpeed;

            enemies[i].rotation += turnDiff;
        }
    }
}
