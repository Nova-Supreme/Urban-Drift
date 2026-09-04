#include "road.h"
#include <math.h>

// Every number below is in WORLD space (the 2048 x 1536 world the car drives around in), NOT image space. 

void Road_Load(RoadNetwork* network)
{
    network->count = 0;

    network->segs[network->count++] = (RoadSegment){
        ROAD_RECT, (Rectangle){ 458, 131, 64,  74 }, {0,0}, 0.0f }; //top little 

    network->segs[network->count++] = (RoadSegment){
        ROAD_CIRCLE, {0,0,0,0}, (Vector2){ 624, 416 }, 235.0f }; //right 
    network->segs[network->count++] = (RoadSegment){
        ROAD_CIRCLE, {0,0,0,0}, (Vector2){ 351, 416 }, 235.0f }; //left
    network->segs[network->count++] = (RoadSegment){
        ROAD_RECT, (Rectangle){ 322, 181, 307, 60 }, {0,0}, 0.0f }; //top
    network->segs[network->count++] = (RoadSegment){
        ROAD_RECT, (Rectangle){ 322, 591, 307, 60 }, {0,0}, 0.0f }; //bottom

    network->segs[network->count++] = (RoadSegment){
        ROAD_RECT, (Rectangle){ 441, 650, 100, 67 }, {0,0}, 0.0f };
    network->segs[network->count++] = (RoadSegment){
        ROAD_SEMI, {0,0,0,0}, (Vector2){ 541, 715 }, 100.0f, 55.0f, 180.0f };
    network->segs[network->count++] = (RoadSegment){
        ROAD_RECT, (Rectangle){ 545, 752, 59, 63 }, {0,0}, 0.0f };
    network->segs[network->count++] = (RoadSegment){
        ROAD_SEMI, {0,0,0,0}, (Vector2){ 604, 863 }, 110.0f, -90.0f, 90.0f };



    network->segs[network->count++] = (RoadSegment){
        ROAD_RECT, (Rectangle){ 487, 1091, 1229, 64 }, {0,0}, 0.0f };

    // network->segs[network->count++] = (RoadSegment){
    //     ROAD_RECT, (Rectangle){ 1696, 1118, 54, 152 }, {0,0}, 0.0f, 0.0f, 0.0f };
}

bool Road_Contains(RoadNetwork* network, Vector2 point)
{
    // Check the point against every road piece. If it falls inside ANY piece
    // then it counts as being on the road.
    for (int i = 0; i < network->count; i++)
    {
        RoadSegment seg = network->segs[i];

        if (seg.type == ROAD_RECT)
        {
            if (CheckCollisionPointRec(point, seg.rect))
                return true;
        }
        else if (seg.type == ROAD_CIRCLE)
        {
            // The point must simply be within the circle's radius.
            Vector2 diff = {
                point.x - seg.center.x,
                point.y - seg.center.y
            };
            float distance = sqrtf(diff.x*diff.x + diff.y*diff.y);
            if (distance <= seg.radius)
                return true;
        }
        else // ROAD_SEMI
        {
            Vector2 diff = {
                point.x - seg.center.x,
                point.y - seg.center.y
            };
            float distance = sqrtf(diff.x*diff.x + diff.y*diff.y);
            if (distance > seg.radius)
                continue; // outside the radius - definitely not on this piece

            // The point is within the radius, but a semicircle only covers part
            // of the circle, so it must ALSO sit between the two angles.
            // Work out where the point is, as an angle from the centre.
            float pointAngle = atan2f(diff.y, diff.x) * RAD2DEG; // -180..180
            if(pointAngle < 0.0f) pointAngle += 360.0f;          // make 0..360

            // Sweep = how far the arc turns (0 to 360 degrees).
            float sweep = seg.endAngle - seg.startAngle;
            if(sweep < 0.0f) sweep += 360.0f;

            // How far past the start angle is this point (0..360).
            float alongArc = fmodf(pointAngle - seg.startAngle, 360.0f);
            if(alongArc < 0.0f) alongArc += 360.0f;

            if(alongArc <= sweep)
                return true;
        }
    }

    return false;
}
