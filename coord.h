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
    unsigned int x;
    unsigned int y;
};

// prototypes
void init_coord(Coord *coord);
void reset_coord(Coord *coord);
Bool is_same_coord(Coord const a, Coord const b);
Bool is_colliding(Coord const goal, unsigned int** const collision_map, Bool const pixel);
Bool is_out_of_map(Coord const goal, Coord const max_coord);

#endif // define RPG_COORD
