#ifndef RPG_SCREEN
#define RPG_SCREEN

#include "errors.h"
#include "try_throw_catch.h"
#include "macros.h"

#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>


// prototypes
void init_screen(SDL_Surface *(*screen));
void set_BG_color(SDL_Surface *(*screen), SDL_Rect* const tile, unsigned int* const rgb);
void make_check_board(SDL_Surface *(*screen), unsigned int const x, unsigned int const y);

// SDL_Surface tile will change for an array once more tiles are included
void apply_tiles(SDL_Surface *(*screen), MapType const type, Map const map, SDL_Surface* const tile);

#endif // define RPG_SCREEN
