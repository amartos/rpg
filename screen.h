#ifndef RPG_SCREEN
#define RPG_SCREEN

#include "errors.h"
#include "try_throw_catch.h"
#include "macros.h"

#include "coord.h"
#include "map.h"
#include "images.h"

#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>


// prototypes
void init_screen(SDL_Window **window, SDL_Renderer **renderer);
// void make_check_board(SDL_Renderer **renderer, unsigned int const x, unsigned int const y);

// SDL_Surface tile will change for an array once more tiles are included
void apply_tiles(SDL_Renderer **renderer, MapType const type, Map const map, Image tiles[0xFFFF], int const xscroll, int const yscroll);

#endif // define RPG_SCREEN
