#ifndef RPG_PATHFINDING
#define RPG_PATHFINDING

#include "errors.h"
#include "macros.h"

#include "navigation.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Maximum number of nodes to go through
#define MAX_PATH_NODES 1000

/* A* pathfinding
 * Returns 0 if no path was found, or the number of nodes if it found one, and
 * sets the nodes to go through in order within the "path" array */
unsigned int find_path(Coord path[MAX_PATH_NODES], Coord const start, Coord const goal, Map const map);

#endif // define RPG_PATHFINDING
