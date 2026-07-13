#ifndef AICAR_H
#define AICAR_H

#include <raylib.h>
#include <stdbool.h>

#define MAX_AICARS 5

typedef struct {
    Vector2 pos;
    float rotation;   // heading, degrees, same convention as player
    float speed;      // constant forward speed
    float radius;     // collision radius
    int textureIndex; // which of cartextures[] to draw
    bool active;      // false once it has left the world / despawned
} AICar;

// Spawns (or respawns) a single AI car at a random valid position with a random heading/speed.
void SpawnSingleAICar(AICar *car, float worldwidth, float worldheight, float radius);

// (Re)spawns all AI cars at once - used for the initial fill and on restart.
void SpawnAICars(AICar aicars[], int count, float worldwidth, float worldheight, float radius);

// Moves every active AI car forward along its heading. Any car that would cross
// the world border is deactivated (disappears) instead of bouncing.
void UpdateAICars(AICar aicars[], int count, float worldwidth, float worldheight, float dt);

// Call once per frame during normal play. Every 'interval' seconds it refills
// exactly one inactive slot, so cars trickle back in rather than reappearing all at once.
// 'timer' is caller-owned state (pass a pointer to a persistent float you reset on restart).
void PeriodicRespawnAICars(AICar aicars[], int count, float worldwidth, float worldheight,
                            float radius, float dt, float *timer, float interval);

// Draws every active AI car using the shared vehicle sprite sheet.
void DrawAICars(AICar aicars[], int count, Texture2D cartextures[], float carWidth, float carHeight);

// Returns true if any active AI car's radius overlaps the player's (circle-circle check).
bool CheckAICarCollision(AICar aicars[], int count, Vector2 playerPos, float playerRadius);

#endif
