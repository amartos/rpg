#ifndef RPG_PATHFINDING_TEST
#define RPG_PATHFINDING_TEST

#include "../errors.h"
#include "../database.h"

#include "../screen.h"
#include "../images.h"

#include "../navigation.h"
#include "../pathfinding.h"

#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <math.h>


#define STARTX 5
#define STARTY 5
#define BASE_TEST_TILES_WIDTH 16
#define BASE_TEST_TILES_HEIGHT 16
// Autoscale tiles sizes with camera settings
#define TEST_TILES_WIDTH(c) scale_size(BASE_TEST_TILES_WIDTH, c)
#define TEST_TILES_HEIGHT(c) scale_size(BASE_TEST_TILES_HEIGHT, c)

// Macros used to enclose time limited events
#define TIME \
        time = (int)SDL_GetTicks(); \
        if (time - prev_time > FRAMERATE) \
        { \
            prev_time = time;

#define ENDTIME \
        } \
        else \
            SDL_Delay((Uint32)abs(FRAMERATE - (time - prev_time))); // do not overuse CPU


#endif // define RPG_PATHFINDING_TEST
