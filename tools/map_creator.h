#ifndef RPG_MAP_CREATOR
#define RPG_MAP_CREATOR

#include "../errors.h"
#include "../try_throw_catch.h"
#include "../macros.h"
#include "../database.h"

#include "../navigation.h"
#include "../assets.h"
#include "../animation.h"
#include "../images.h"
#include "../movement.h"
#include "../screen.h"

#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <unistd.h> // used for optargs

#define MAX_SIZE_LINE 1000

#define ZLOOP(i, text, value) \
    for (z=i;z<MAX_LEVELS;z++) \
    { \
        sprintf(level_value, text, value); \
        strcat(query, level_value); \
    }

// Macros used to enclose time limited events
#define TIME \
        time = SDL_GetTicks(); \
        if (time - prev_time > FRAMERATE) \
        { \
            prev_time = time;

#define ENDTIME \
        } \
        else \
            SDL_Delay(FRAMERATE - (time - prev_time)); // do not overuse CPU

enum {
    ID_FLOOR = 0x4000,
    ID_WALL0 = 0x0400,
    ID_WALL1 = 0x4001
};

#endif // define RPG_MAP_CREATOR
