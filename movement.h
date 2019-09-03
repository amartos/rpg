#ifndef RPG_MOVEMENT
#define RPG_MOVEMENT

#include "errors.h"
#include "try_throw_catch.h"
#include "macros.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>


// prototypes
void init_coord(Coord *coord);
Coord offsetting(Coord const position);
void get_formation_offset(
        Coord *position,
        unsigned int const char_number,
        Deployment const deployment
        );
Bool is_same_coord(Coord const a, Coord const b);
Bool is_colliding(Coord const goal, unsigned int** const collision_map, Bool const pixel);
Bool is_out_of_map(Coord const goal, Coord const max_coord);
unsigned int find_path(
        Coord *(*path),
        Coord const start,
        Coord goal,
        unsigned int const velocity,
        Coord const max_coord,
        unsigned int** const collision_map,
        unsigned int** const movement_cost_map
        );
Cardinals move(
        Coord *start,
        Coord const goal,
        MovementType const type,
        Coord const max_coord,
        unsigned int** const collision_map,
        unsigned int const velocity
        );

#endif // define RPG_MOVEMENT
