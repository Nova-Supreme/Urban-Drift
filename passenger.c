#include "passenger.h"

void Passenger_Update(Passenger* passenger, Vector2 playerpos, float playerwidth, float playerspeed, int* money)
{
    if(!passenger->isspawned) return;

    if(!passenger->ispickedup){
        if(CheckCollisionCircles(playerpos, playerwidth/3.0f, passenger->position, passenger->interactionradius) && playerspeed==0.0f){
            passenger->ispickedup = true;
        }
    }
    else{
        if(CheckCollisionCircles(playerpos, playerwidth/3.0f, passenger->destination, passenger->interactionradius) && playerspeed==0.0f){
            passenger->ispickedup = false;
            passenger->isspawned = false;
            *money += 100;
        }
    }
}
