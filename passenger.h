#ifndef PASSENGER_H
#define PASSENGER_H

#include <raylib.h>
#include "cars.h"
#include "road.h"

// How many passengers can be on the road (waiting or carried) at the same time.
// More than one can be picked up, so several destinations can be active at once.
#define MAX_PASSENGERS 3

// Updates all passengers for one frame:
//   - picks up a waiting passenger when the player stops right on them, as long
//     as the car still has room (maxCarrying = the vehicle's maxpassengers), and
//   - drops off a carried passenger when the player stops at their destination,
//     giving money each time.
// Returns how many passengers are currently riding in the car.
int Passenger_Update(Passenger* passengers, int count, Vector2 playerpos, float playerwidth, float playerspeed, int* money, int maxCarrying);

// Places a brand-new passenger at a random point on the road, with a random
// destination (also on the road) far enough away to be a real trip.
void Passenger_SpawnRandom(Passenger* passenger, RoadNetwork* road, float worldwidth, float worldheight);

#endif
