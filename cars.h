#ifndef CAR_H //asks compiler if it has looked at a file named CAR_H during this build
#define CAR_H //if no then it marks CAR_H as defined and tells it to read the contents of this file
#include <raylib.h>

typedef struct vehicle{
    int id;
    float maxspeed;
    float acceleration;
    float friction;
    int maxpassengers;
    float width;
    float height;
    float brakeforce;
} vehicle;

vehicle GetVehiclePreset(int choice); //function prototype

typedef struct passenger{
    Vector2 position;
    Vector2 destination;
    bool isspawned;
    bool ispickedup;
    float interactionradius;
} Passenger;


#endif