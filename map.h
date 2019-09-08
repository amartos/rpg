#ifndef RPG_MAP
#define RPG_MAP

#include "errors.h"
#include "try_throw_catch.h"
#include "macros.h"

#include <stdio.h>
#include <stdlib.h>

// structures
// The different types of maps found in a map file
// Background always fist, weather always last
typedef enum MapType MapType;
enum MapType {
    BACKGROUND,
    FOREGROUND,
    COLLISIONS,
    COST,
    WEATHER
};

// Maps structures
typedef struct Map Map;
struct Map
{
    unsigned int maxx, maxy; // max number of tiles for x and y
    unsigned int ***schematics;
};

// prototypes
void init_map(Map *map, char const map_path[]);
void free_map(Map *map);
void init_empty_map(Map *map, unsigned int const maxx, unsigned int const maxy);

#endif // define RPG_MAP
