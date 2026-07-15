#ifndef PASSENGER_H
#define PASSENGER_H

#include <raylib.h>
#include "cars.h"

// Checks pickup/dropoff collisions and updates passenger state + money.
void Passenger_Update(Passenger* passenger, Vector2 playerpos, float playerwidth, float playerspeed, int* money);

#endif
