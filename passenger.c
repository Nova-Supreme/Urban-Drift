#include "passenger.h"

// Counts how many passengers are currently riding in the car.
static int CarriedCount(Passenger* passengers, int count)
{
    int carried = 0;
    for(int i=0;i<count;i++)
        if(passengers[i].isspawned && passengers[i].ispickedup) carried++;
    return carried;
}

int Passenger_Update(Passenger* passengers, int count, Vector2 playerpos, float playerwidth, float playerspeed, int* money, float* moneyMultiplier, int maxCarrying)
{
    int carried = CarriedCount(passengers, count);

    for(int i=0;i<count;i++){
        Passenger* p = &passengers[i];
        if(!p->isspawned) continue;

        if(!p->ispickedup){
            // A person waiting on the road. Pick them up if the player stops
            // right on them AND the car still has room for another passenger,
            // so several can be carried at the same time.
            if(carried < maxCarrying &&
               CheckCollisionCircles(playerpos, playerwidth/3.0f, p->position, p->interactionradius) && playerspeed==0.0f){
                p->ispickedup = true;
                carried++;
            }
        }
        else{
            // We have them; dropping them off at their destination earns money.
            if(CheckCollisionCircles(playerpos, playerwidth/3.0f, p->destination, p->interactionradius) && playerspeed==0.0f){
                p->ispickedup = false;
                p->isspawned = false; // gone - the game will spawn a new one
                // Each drop-off earns $100 times the current money multiplier,
                // then the multiplier steps up 0.1 for the next delivery.
                *money += (int)(100.0f * (*moneyMultiplier));
                *moneyMultiplier += 0.1f;
                carried--;
            }
        }
    }
    return carried;
}

// Finds a random point that sits ON the road (so the player can always reach
// it). Tries up to 500 random spots and returns the first that is on-road.
static Vector2 RandomRoadPoint(RoadNetwork* road, float worldwidth, float worldheight)
{
    for(int i=0;i<500;i++){
        Vector2 p = {
            (float)GetRandomValue(0, (int)worldwidth),
            (float)GetRandomValue(0, (int)worldheight)
        };
        if(Road_Contains(road, p)) return p;
    }
    // Safety net (600 random spots all missed) - fall back to the player start,
    // which is on the road in practice.
    return (Vector2){170.0f, 350.0f};
}

void Passenger_SpawnRandom(Passenger* passenger, RoadNetwork* road, float worldwidth, float worldheight)
{
    // Pick a pickup point on the road.
    passenger->position = RandomRoadPoint(road, worldwidth, worldheight);

    // Pick a destination that is on the road and reasonably far from the pickup
    // so the trip is actually a drive, not a one-metre hop.
    int attempts = 0;
    Vector2 destination = passenger->position;
    while(attempts < 200){
        destination = RandomRoadPoint(road, worldwidth, worldheight);
        float dx = destination.x - passenger->position.x;
        float dy = destination.y - passenger->position.y;
        if((dx*dx + dy*dy) > (250.0f*250.0f)) break;
        attempts++;
    }
    passenger->destination = destination;

    passenger->isspawned = true;
    passenger->ispickedup = false;
    passenger->interactionradius = 40.0f;
}
