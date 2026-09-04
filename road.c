#include "road.h"
#include <math.h>

// Every number below is in WORLD space (the 2048 x 1536 world the car drives
// around in), NOT image space. They were measured directly from the grey
// asphalt pixels in map0.png: the road winds down the middle of the map,
// then a horizontal road crosses the bottom, and a straight road runs down
// the right-hand side.

void Road_Load(RoadNetwork* network)
{
    network->count = 0;

    // ---- Central vertical road (the one the player starts on) ----

    // Small narrow spur at the very top that leads up to the first pickup.
    network->segs[network->count++] = (RoadSegment){
        ROAD_RECT, (Rectangle){ 458, 131, 64,  74 }, {0,0}, 0.0f };

    // The central road, split into narrow stacked rectangles that follow its
    // S-shaped curve. The road is wide near the middle and pinches narrow
    // lower down, so these bands get narrower as they go.
    network->segs[network->count++] = (RoadSegment){
        ROAD_RECT, (Rectangle){ 300, 186, 390,  80 }, {0,0}, 0.0f };
    network->segs[network->count++] = (RoadSegment){
        ROAD_RECT, (Rectangle){ 150, 266, 691, 107 }, {0,0}, 0.0f };
    network->segs[network->count++] = (RoadSegment){
        ROAD_RECT, (Rectangle){ 150, 373, 721, 107 }, {0,0}, 0.0f };
    network->segs[network->count++] = (RoadSegment){
        ROAD_RECT, (Rectangle){ 150, 479, 721, 107 }, {0,0}, 0.0f };
    network->segs[network->count++] = (RoadSegment){
        ROAD_RECT, (Rectangle){ 225, 586, 571,  80 }, {0,0}, 0.0f };
    network->segs[network->count++] = (RoadSegment){
        ROAD_RECT, (Rectangle){ 451, 666, 150, 107 }, {0,0}, 0.0f };
    network->segs[network->count++] = (RoadSegment){
        ROAD_RECT, (Rectangle){ 451, 772, 271, 160 }, {0,0}, 0.0f };
    network->segs[network->count++] = (RoadSegment){
        ROAD_RECT, (Rectangle){ 383, 932, 249, 160 }, {0,0}, 0.0f };

    // ---- Bottom horizontal road (crosses the whole map) ----
    network->segs[network->count++] = (RoadSegment){
        ROAD_RECT, (Rectangle){ 421, 1091, 1322, 81 }, {0,0}, 0.0f };

    // ---- Straight road running down the right-hand side ----
    network->segs[network->count++] = (RoadSegment){
        ROAD_RECT, (Rectangle){ 1683, 1118, 150, 227 }, {0,0}, 0.0f };

    // ---- Circles for the curved bends / junctions between the roads ----
    // Upper S-bend of the central road.
    network->segs[network->count++] = (RoadSegment){
        ROAD_CIRCLE, {0,0,0,0}, (Vector2){ 586, 640 }, 95.0f };
    // Lower S-bend of the central road.
    network->segs[network->count++] = (RoadSegment){
        ROAD_CIRCLE, {0,0,0,0}, (Vector2){ 420, 890 }, 100.0f };
    // Junction of the central road and the bottom horizontal road.
    network->segs[network->count++] = (RoadSegment){
        ROAD_CIRCLE, {0,0,0,0}, (Vector2){ 511, 1118 }, 133.0f };
    // Junction of the bottom horizontal road and the right-side road.
    network->segs[network->count++] = (RoadSegment){
        ROAD_CIRCLE, {0,0,0,0}, (Vector2){ 1713, 1138 }, 126.0f };
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
        else // ROAD_CIRCLE
        {
            Vector2 diff = {
                point.x - seg.center.x,
                point.y - seg.center.y
            };
            float distance = sqrtf(diff.x*diff.x + diff.y*diff.y);
            if (distance <= seg.radius)
                return true;
        }
    }

    return false;
}
