#ifndef RPG_ERRORS
#define RPG_ERRORS

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// prototypes
void logger(const int error_id, const char sdl_message[]);

// custom errors
enum Errors
{
    SCREEN_INIT_FAILURE = 1,
    VIDEO_MODE_LOAD_FAILURE,
    SPRITE_LOAD_FAILURE,
    FLIP_SCREEN_FAILURE,
    CHARACTER_NOT_ANIMATED,
    CHARACTER_NOT_MOVING
};

#endif // define RPG_ERROR
