#ifndef RPG_COORD
#define RPG_COORD

#include "errors.h"
#include "try_throw_catch.h"
#include "macros.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// structures
// the order of the sprites on the asset img depends on this
typedef enum Cardinals Cardinals;
enum Cardinals {N, NE, E, SE, S, SW, W, NW};

typedef struct Coord Coord;
struct Coord
{
    double x;
    double y;
    Bool pixels;
};

// prototypes
void init_coord(Coord *coord);
void reset_coord(Coord *coord);
void pixels_to_unit(Coord *coord);
void unit_to_pixels(Coord *coord);
void round_coord(Coord *coord);
Coord isometric_to_cartesian(Coord const isometric);
Coord cartesian_to_isometric(Coord const cartesian);
Bool is_same_coord(Coord const a, Coord const b);
Bool is_colliding(Coord const goal, unsigned int** const collision_map);
Bool are_corners_colliding(Coord const start, Coord const goal, unsigned int** const collision_map);
Bool is_out_of_map(Coord const goal, Coord const max_coord);
Cardinals determine_direction(Coord const start, Coord const goal);

#endif // define RPG_COORD
