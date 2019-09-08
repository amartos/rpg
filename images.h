#ifndef RPG_IMAGES
#define RPG_IMAGES

#include "errors.h"
#include "try_throw_catch.h"
#include "macros.h"

#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

// prototypes
void set_color(SDL_Surface *image, SDL_Color const original_color, SDL_Color const new_color);

#endif // define RPG_IMAGES
