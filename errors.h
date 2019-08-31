#ifndef RPG_ERRORS
#define RPG_ERRORS

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// prototypes
void logger(const int error_id, const char sdl_message[]);

// structures
// custom errors
enum Errors
{
    SCREEN_INIT_FAILURE = 1,
    VIDEO_MODE_LOAD_FAILURE,
    SPRITE_LOAD_FAILURE,
    FLIP_SCREEN_FAILURE,
    CHARACTER_NOT_ANIMATED,
    CHARACTER_NOT_MOVING,
    MAP_FILE_LOADING_FAILURE,
    CONVERTER_FILE_LOADING_FAILURE,
    MAP_MALLOC_FAILURE,
    PATHFIND_MALLOC_FAILURE
};

#endif // define RPG_ERROR
