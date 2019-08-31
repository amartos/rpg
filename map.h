#ifndef RPG_MAP
#define RPG_MAP

#define MAX_SIZE_LINE 1000

#include "errors.h"
#include "try_throw_catch.h"
#include "macros.h"

#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>


// structures
// Background always fist, weather always last
typedef enum MapType MapType;
enum MapType {
    BACKGROUND,
    FOREGROUND,
    COLLISIONS,
    COST,
    WEATHER
};

// This is an embryo of a tile structure, unused yet
typedef struct Tile Tile;
struct Tile
{
    SDL_Surface *image;
    int collision;
};

typedef struct Map Map;
struct Map
{
    unsigned int w, h;
    unsigned int x_tiles, y_tiles;
    unsigned int total_tiles;
    unsigned int xscroll, yscroll;
    unsigned int ***schematics;
};

// prototypes
void init_map(Map *map, const char map_path[]);
void free_map(Map *map);

#endif // define RPG_MAP
