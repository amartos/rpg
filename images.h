#ifndef RPG_IMAGES
#define RPG_IMAGES

#define SPRITES_WIDTH 32
#define SPRITES_HEIGHT 32
#define COLOR_PALETTE 3

#include "macros.h"

#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

// prototypes
void set_color(SDL_Surface *image, SDL_Color const original_color, SDL_Color const new_color);

#endif // define RPG_IMAGES
