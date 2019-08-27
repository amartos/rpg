#ifndef RPG_MAP
#define RPG_MAP

#define MAX_SIZE_LINE 1000
#define TILES_WIDTH 32
#define TILES_HEIGHT 32

#include "errors.h"
#include "try_throw_catch.h"

#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>


// structures

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
    /* To get the width and height of each unit,
     * divide total width or height by x or y */
    SDL_Rect infos;

    unsigned int **background_tiles;
    unsigned int **foreground_tiles;
    unsigned int **collisions;
    unsigned int **weather;
};


// prototypes
void init_map(Map *map, const char map_path[]);
void free_map(Map *map);

#endif // define RPG_MAP
