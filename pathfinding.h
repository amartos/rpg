#ifndef RPG_PATHFINDING
#define RPG_PATHFINDING

#include "movement.h"

#include "errors.h"
#include "try_throw_catch.h"
#include "macros.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// A* pathfinding
unsigned int find_path(
        Coord *(*path),
        Coord const start_pixels,
        Coord goal_pixels,
        unsigned int const velocity,
        Coord const max_coord_pixels,
        unsigned int** const collision_map,
        unsigned int** const movement_cost_map
        );

#endif // define RPG_PATHFINDING
