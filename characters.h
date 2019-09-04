#ifndef RPG_CHARACTERS
#define RPG_CHARACTERS

#include "errors.h"
#include "try_throw_catch.h"
#include "macros.h"
#include "images.h"
#include "coord.h"
#include "movement.h"

#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
// this should be removed at some point
#include <SDL/SDL_rotozoom.h>


// prototypes
// Character initialization. For now the values are manually set, but in the
// future all will depend on the id
void init_character(Character *character, unsigned int const id, Coord const start_position, Deployment const formation);
void free_character(Character *character);

#endif // define RPG_CHARACTERS
