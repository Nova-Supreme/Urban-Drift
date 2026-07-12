#include "cars.h"
vehicle GetVehiclePreset(int choice){
    vehicle temp;
    switch(choice){
        case 1: //rickshaw (starter)
            temp.id=1;
            temp.maxspeed=1.2f;
            temp.acceleration=0.0025f;
            temp.deceleration=0.0075f;
            temp.maxpassengers=2;
            temp.width=25;
            temp.height=50;
            break;

        case 2: //cng
            temp.id=2;
            temp.maxspeed=1.75f;
            temp.acceleration=0.0040f;
            temp.deceleration=0.0090f;
            temp.maxpassengers=3;
            temp.width=30;
            temp.height=50;
            break;

        case 3: //auto
            temp.id=3;
            temp.maxspeed=1.5f;
            temp.acceleration=0.003f;
            temp.deceleration=0.0075f;
            temp.maxpassengers=4;
            temp.width=35;
            temp.height=55;
            break;

        case 4: //personal car (goal)
            temp.id=4;
            temp.maxspeed=4.0f;
            temp.acceleration=0.02f;
            temp.deceleration=0.035f;
            temp.maxpassengers=4;
            temp.width=50;
            temp.height=80;
            break;
    }
    temp.friction=0.005;
    return temp;
}