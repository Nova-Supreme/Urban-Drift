#include "road.h"
#include <math.h>

// Every number below is in WORLD space (map1's world is 3072 x 2304, map0's is
// 2048 x 1536 - the car drives around these rectangles), NOT image space. 

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
        ROAD_RECT, (Rectangle){ 545, 747, 59, 68 }, {0,0}, 0.0f };
    network->segs[network->count++] = (RoadSegment){
        ROAD_SEMI, {0,0,0,0}, (Vector2){ 604, 863 }, 115.0f, -125.0f, 120.0f };
    network->segs[network->count++] = (RoadSegment){
        ROAD_RECT, (Rectangle){ 500, 915, 105, 63 }, {0,0}, 0.0f };
    network->segs[network->count++] = (RoadSegment){
        ROAD_SEMI, {0,0,0,0}, (Vector2){ 500, 1035 }, 120.0f, 60.0f, 300.0f };


    network->segs[network->count++] = (RoadSegment){
        ROAD_RECT, (Rectangle){ 487, 1091, 1229, 64 }, {0,0}, 0.0f };

    network->segs[network->count++] = (RoadSegment){
        ROAD_SEMI, {0,0,0,0}, (Vector2){ 1697, 1190 }, 99.0f, -120.0f, 90.0f };
}

void Road_LoadMap1(RoadNetwork* network)
{
    // map1.png is 1363 x 1154 pixels inside a world of 3072 x 2304, so a
    // position at image pixel (px, py) sits at world:
    //   worldX = px * 3072/1363  (about px * 2.25)
    //   worldY = py * 2304/1154  (about py * 2.00)
    //
    // Layout: ONE big rectangular circuit around the town:
    //   - a long horizontal road along the bottom (the "trunk"),
    //   - a matching horizontal road along the top,
    //   - a vertical road up the LEFT side and one up the RIGHT side,
    // and each corner is a full circle so cars turn smoothly.
    // At each corner two roads overlap, so the circle lets you swing through
    // the turn without leaving the asphalt.
    network->count = 0;
    network->segs[network->count++] = (RoadSegment){
        ROAD_RECT, (Rectangle){ 430, 223, 565,60 }, {0,0}, 0.0f };
    network->segs[network->count++] = (RoadSegment){
        ROAD_RECT, (Rectangle){ 400, 780, 678 ,60}, {0,0}, 0.0f };
    network->segs[network->count++] = (RoadSegment){
        ROAD_RECT, (Rectangle){ 1040, 350, 85 ,1560}, {0,0}, 0.0f };
    network->segs[network->count++] = (RoadSegment){
        ROAD_RECT, (Rectangle){ 329, 340 , 78 ,1580 }, {0,0}, 0.0f };
    network->segs[network->count++] = (RoadSegment){
        ROAD_RECT, (Rectangle){ 400, 1410, 678 ,60}, {0,0}, 0.0f };
    network->segs[network->count++] = (RoadSegment){
        ROAD_RECT, (Rectangle){ 460, 1960, 540 ,65}, {0,0}, 0.0f };
    network->segs[network->count++] = (RoadSegment){
        ROAD_RECT, (Rectangle){ 1100, 1095, 1480, 68}, {0,0}, 0.0f };
    network->segs[network->count++] = (RoadSegment){
        ROAD_RECT, (Rectangle){ 2202, 777, 75 ,780}, {0,0}, 0.0f };
    network->segs[network->count++] = (RoadSegment){
        ROAD_RECT, (Rectangle){ 2305, 650, 350,60}, {0,0}, 0.0f };
    network->segs[network->count++] = (RoadSegment){
        ROAD_RECT, (Rectangle){ 2325, 1602, 295,70}, {0,0}, 0.0f };
    network->segs[network->count++] = (RoadSegment){
        ROAD_RECT, (Rectangle){ 2670, 564, 65,50}, {0,0}, 0.0f };
    network->segs[network->count++] = (RoadSegment){
        ROAD_RECT, (Rectangle){ 2670, 1665, 65,120}, {0,0}, 0.0f };


    network->segs[network->count++] = (RoadSegment){
        ROAD_SEMI, {0,0,0,0}, (Vector2){ 2303, 752 },  100.0f, 130.0f, 300.0f };
    network->segs[network->count++] = (RoadSegment){
        ROAD_SEMI, {0,0,0,0}, (Vector2){ 1017, 333 },  110.0f, 60.0f, -120.0f };
    network->segs[network->count++] = (RoadSegment){
        ROAD_SEMI, {0,0,0,0}, (Vector2){ 464, 810 },  110.0f, 130.0f, 225.0f };
    network->segs[network->count++] = (RoadSegment){
        ROAD_SEMI, {0,0,0,0}, (Vector2){ 995, 810 },  110.0f, -50.0f, 50.0f };
    network->segs[network->count++] = (RoadSegment){
        ROAD_SEMI, {0,0,0,0}, (Vector2){ 995, 1440 },  110.0f, -50.0f, 55.0f };
    network->segs[network->count++] = (RoadSegment){
        ROAD_SEMI, {0,0,0,0}, (Vector2){ 464, 1440 },  110.0f, 130.0f, 225.0f };
    network->segs[network->count++] = (RoadSegment){
        ROAD_SEMI, {0,0,0,0}, (Vector2){ 440, 1923 },  105.0f, 40.0f, 230.0f };
    network->segs[network->count++] = (RoadSegment){
        ROAD_SEMI, {0,0,0,0}, (Vector2){ 1017, 1918 },  110.0f, -30.0f, 140.0f };
    network->segs[network->count++] = (RoadSegment){
        ROAD_SEMI, {0,0,0,0}, (Vector2){ 1180, 1126 },  110.0f, 130.0f, 225.0f };
    network->segs[network->count++] = (RoadSegment){
        ROAD_SEMI, {0,0,0,0}, (Vector2){ 442, 333 },  110.0f, 120.0f, 300.0f };
    network->segs[network->count++] = (RoadSegment){
        ROAD_SEMI, {0,0,0,0}, (Vector2){ 1017, 333 },  110.0f, -120.0f, 60.0f };
    network->segs[network->count++] = (RoadSegment){
        ROAD_SEMI, {0,0,0,0}, (Vector2){ 2653, 617 },  90.0f, -30.0f, 120.0f };
    network->segs[network->count++] = (RoadSegment){
        ROAD_SEMI, {0,0,0,0}, (Vector2){ 2315, 1565 },  110.0f, 60.0f, 240.0f };
    network->segs[network->count++] = (RoadSegment){
        ROAD_SEMI, {0,0,0,0}, (Vector2){ 2646, 1700 },  100.0f, -120.0f, 30.0f };
    network->segs[network->count++] = (RoadSegment){
        ROAD_SEMI, {0,0,0,0}, (Vector2){ 2345, 1126 },  110.0f, 130.0f, 225.0f };
    network->segs[network->count++] = (RoadSegment){
        ROAD_SEMI, {0,0,0,0}, (Vector2){ 2135, 1126 },  110.0f, -45.0f, 50.0f };
}

void Road_LoadMap2(RoadNetwork* network)
{
    // Ponytail: map2 is the FIRST map played but its roads are still being
    // drawn. Until they are, the whole world counts as drivable road so the
    // player can test-drive on it. Replace with real roads when the art is in.
    network->count = 0;
    network->segs[network->count++] = (RoadSegment){
        ROAD_RECT, (Rectangle){ 0, 0, 1536, 1152 }, {0,0}, 0.0f };
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
