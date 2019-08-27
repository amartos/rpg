#ifndef RPG_TILES
#define RPG_TILES

#include "macros.h"

#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

// structures

// This is an embryo of a tile structure, unused yet
typedef struct Tile Tile;
struct Tile
{
    SDL_Surface *image;
    int collision;
};

// prototypes
void set_color(SDL_Surface *image, SDL_Color original_color, SDL_Color new_color);

#endif // define RPG_TILES
