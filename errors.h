#ifndef RPG_ERRORS
#define RPG_ERRORS

#include <stdio.h>
#include <stdlib.h>
#include <time.h>


// structures
// custom errors
typedef enum Errors Errors;
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

// prototypes
void logger(const Errors error_id, const char sdl_message[]);

#endif // define RPG_ERROR
