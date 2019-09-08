#ifndef RPG_MAP_CREATOR
#define RPG_MAP_CREATOR

#include "../errors.h"
#include "../try_throw_catch.h"
#include "../macros.h"

#include "../screen.h"
#include "../images.h"
#include "../coord.h"
#include "../map.h"

#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>


enum {
    ID_FLOOR = 0x100,
    ID_WALL0 = 0x101,
    ID_WALL1 = 0x102
};

#endif // define RPG_MAP_CREATOR
