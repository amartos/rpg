#ifndef RPG_TILES
#define RPG_TILES

#define COLOR_PALETTE 3

#include "macros.h"

#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

// prototypes
void set_color(SDL_Surface *image, SDL_Color original_color, SDL_Color new_color);

#endif // define RPG_TILES
