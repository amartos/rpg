#ifndef RPG_MAP_CREATOR
#define RPG_MAP_CREATOR

#include "../errors.h"
#include "../try_throw_catch.h"
#include "../macros.h"

#include "../screen.h"
#include "../images.h"
#include "../coord.h"
#include "../map.h"
#include "../assets.h"
#include "../animation.h"
#include "../movement.h"
#include "../database.h"

#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define ZLOOP(i, text, value) \
    for (z=i;z<MAX_LEVELS;z++) \
    { \
        sprintf(level_value, text, value); \
        strcat(query, level_value); \
    }


enum {
    ID_FLOOR = 0x4000,
    ID_WALL0 = 0x0400,
    ID_WALL1 = 0x4001
};

#endif // define RPG_MAP_CREATOR
