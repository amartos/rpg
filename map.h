#ifndef RPG_MAP
#define RPG_MAP

#include "errors.h"
#include "try_throw_catch.h"
#include "macros.h"

#include <stdio.h>
#include <stdlib.h>

#define MAX_LEVELS 2 // ground + floors

// structures
// Maps structures
typedef struct Map Map;
struct Map
{
    unsigned int maxx, maxy; // max number of tiles for x and y
    unsigned int ***tiles;
    unsigned int **collisions;
    unsigned int **cost;
    unsigned int **weather;
};

// prototypes
void init_map(Map *map, char const map_path[]);
void free_map(Map *map);
void init_empty_map(Map *map, unsigned int const maxx, unsigned int const maxy);

#endif // define RPG_MAP
