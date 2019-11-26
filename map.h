#ifndef RPG_MAP
#define RPG_MAP

#include "errors.h"
#include "try_throw_catch.h"
#include "macros.h"
#include "database.h"

#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>

// Maximum number of floors defined in each map.
// This counts the ground + other floors.
// Note that for isometric depth, more than 1 floor needs to be defined to make
// walls.
#define MAX_LEVELS 3

typedef struct Map Map;
struct Map
{
    unsigned int maxx, maxy; // max number of tiles for x and y
    unsigned int ***tiles; // image id for each tile
    // This array is, for now, used as a Bool to determine collision (1 is
    // colliding, 0 is not). In the future, different types of collision might
    // be defined, thus needing ids instead of Bool.
    // Moreover, using the same type as the others arrays of the Map simplifies
    // code for malloc.
    unsigned int **collisions; // Bool 
    unsigned int **cost; // movement cost for each tile
    // Weather ID for each tile. This might be modified in the future to
    // integrate a third dimension (or more).
    unsigned int **weather;
};

// The main initialization function. This NEEDS the map to be fully defined in
// the DB which path is defined in the database module. The map_name is used as
// a table name, and an entry key in the maps table.
// To know more about the DB tables, see the map_crator module.
void init_map(Map *map, char map_name[]);
void free_map(Map *map);

// This function is used with the map_creator tool, to initialize an empty map
// of defined size. This is used only for malloc, as the map does not need to be
// in the DB.
void init_empty_map(Map *map, unsigned int const maxx, unsigned int const maxy);

// This function frees all arrays of the Map struct.
void free_map(Map *map);

#endif // define RPG_MAP
