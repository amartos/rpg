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
// returns the number of nodes it has found for the path, and modifies the
// Character structure's path pointer to point to the corresponding Coord array
unsigned int find_path(Movement *movement, Map const map);

#endif // define RPG_PATHFINDING
