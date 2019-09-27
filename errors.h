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
    VIDEO_INIT_FAILURE = 1,
    WINDOW_INIT_FAILURE,
    VIDEO_MODE_LOAD_FAILURE,
    SPRITE_LOAD_FAILURE,
    CHARACTER_NOT_ANIMATED,
    CHARACTER_NOT_MOVING,
    MAP_FILE_LOADING_FAILURE,
    CONVERTER_FILE_LOADING_FAILURE,
    MAP_MALLOC_FAILURE,
    PATHFIND_MALLOC_FAILURE,
    TILES_PATH_MALLOC_FAILURE
};

// prototypes
void logger(Errors const error_id, char const sdl_message[]);

#endif // define RPG_ERROR
