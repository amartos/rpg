#ifndef RPG_CHARACTERS
#define RPG_CHARACTERS

#include "errors.h"
#include "try_throw_catch.h"
#include "macros.h"
#include "images.h"

#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>


// prototypes
void init_character(
        Character *character,
        SDL_Color const colors[],
        char const sprite_path[],
        unsigned int const number_of_frames,
        unsigned int const fps,
        unsigned int const velocity,
        Coord const start_position
        );
void free_character(Character *character);
void free_path(Character *character);

#endif // define RPG_CHARACTERS
