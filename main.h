#ifndef RPG_MAIN
#define RPG_MAIN

#include "errors.h"
#include "try_throw_catch.h"
#include "macros.h"

#include "navigation.h"
#include "screen.h"
#include "animation.h"
#include "movement.h"
#include "event_handler.h"
#include "assets.h"
#include "database.h"

#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <sqlite3.h>


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


#endif // define RPG_MAIN
