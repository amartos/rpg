#ifndef RPG_MOVEMENT
#define RPG_MOVEMENT

#include "errors.h"
#include "try_throw_catch.h"
#include "macros.h"

#include "coord.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>


// prototypes
void formation_offsetting(
        Coord *position,
        unsigned int const char_number,
        Deployment const deployment
        );
void move(Movement *movement, Coord const max_coord, unsigned int** const collision_map);

#endif // define RPG_MOVEMENT
