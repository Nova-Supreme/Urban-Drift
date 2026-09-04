#ifndef ROAD_H
#define ROAD_H

#include <raylib.h>
#include <stdbool.h>

// The drivable road is built out of small pieces. Each piece is either:
//   - ROAD_RECT   : a plain rectangle (for straight road stretches)
//   - ROAD_CIRCLE : a full circle centred at `center` (for curves/junctions)
//   - ROAD_SEMI   : part of a circle from startAngle to endAngle, in DEGREES
//                   (for half-circle turns). 0 deg = right, 90 = down,
//                   180 = left, 270 = up. A semicircle is 180 degrees of arc.
typedef enum { ROAD_RECT, ROAD_CIRCLE, ROAD_SEMI } RoadType;

typedef struct {
    RoadType type;
    Rectangle rect;     // used when type == ROAD_RECT
    Vector2   center;   // used for ROAD_CIRCLE and ROAD_SEMI
    float     radius;   // used for ROAD_CIRCLE and ROAD_SEMI
    float     startAngle; // used for ROAD_SEMI only (degrees)
    float     endAngle;   // used for ROAD_SEMI only (degrees)
} RoadSegment;

// The biggest the road is allowed to get. If you add more pieces than this,
// raise the number.
#define MAX_ROAD_SEGMENTS 16

typedef struct {
    RoadSegment segs[MAX_ROAD_SEGMENTS];
    int count;
} RoadNetwork;

// Fills `network` with the road layout for map0. Call this once at startup.
void Road_Load(RoadNetwork* network);

// Returns true if `point` sits on top of any road piece (i.e. is on the road).
bool Road_Contains(RoadNetwork* network, Vector2 point);

#endif
