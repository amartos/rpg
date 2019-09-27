#ifndef RPG_SCREEN
#define RPG_SCREEN

#include "errors.h"
#include "try_throw_catch.h"
#include "macros.h"

#include "coord.h"
#include "map.h"
#include "images.h"
#include "characters.h"

#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>


// prototypes
void init_screen(SDL_Window **window, SDL_Renderer **renderer);
void init_sdl_rect(SDL_Rect *rect);
// void make_check_board(SDL_Renderer **renderer, unsigned int const x, unsigned int const y);

// SDL_Surface tile will change for an array once more tiles are included
void apply_tiles(
        SDL_Renderer **renderer,
        Map const map,
        Character all_characters[MAX_CHARACTERS],
        Image images[0xFFFF],
        int const xscroll, int const yscroll
        );

#endif // define RPG_SCREEN
