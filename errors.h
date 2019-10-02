#ifndef RPG_ERRORS
#define RPG_ERRORS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "macros.h"
#include "try_throw_catch.h"

// structures
// custom errors
typedef enum Errors Errors;
enum Errors
{
    VIDEO_INIT_FAILURE = 1,
    WINDOW_INIT_FAILURE,
    MAP_MALLOC_FAILURE,
    PATHFIND_MALLOC_FAILURE,
    DATABASE_READ_FAILURE,
    QUERY_READ_FAILURE,
    QUERY_END_FAILURE,
    IMAGE_MALLOC_FAILURE,
    ANIMATION_MALLOC_FAILURE,
    MOVEMENT_MALLOC_FAILURE,
    ERR_MSG_MALLOC_FAILURE,
    TEXTURE_MALLOC_FAILURE
};

// prototypes
void logger(Errors const error_id, char const sdl_message[]);

#endif // define RPG_ERROR
