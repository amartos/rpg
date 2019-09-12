#ifndef RPG_PATHFINDING
#define RPG_PATHFINDING

#define MAX_PATH_NODES 1000

#include "errors.h"
#include "try_throw_catch.h"
#include "macros.h"

#include "coord.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// A* pathfinding
// returns the number of nodes it has found for the path, and modifies the
// Character structure's path pointer to point to the corresponding Coord array
unsigned int find_path(
        Coord path[MAX_PATH_NODES],
        Coord const start_coord, Coord const goal_coord,
        Coord const max_xy,
        unsigned int** const collision_map,
        unsigned int** const cost_map,
        unsigned int scores[MAX_PATH_NODES]
        );

#endif // define RPG_PATHFINDING
