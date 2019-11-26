#ifndef RPG_ERRORS
#define RPG_ERRORS

#include "macros.h"
#include "try_throw_catch.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* Custom errors definitions
 * Starts at 1 to avoid some errors in the try_throw_catch module that does not
 * handle well starting at 0 (see commit 88a5a63). */
typedef enum Errors Errors;
enum Errors
{
    VIDEO_INIT_FAILURE = 1,
    WINDOW_INIT_FAILURE,
    MAP_MALLOC_FAILURE,
    PATHFIND_MALLOC_FAILURE,
    DATABASE_READ_FAILURE,
    QUERY_READ_FAILURE,
    QUERY_MODIFY_FAILURE,
    QUERY_END_FAILURE,
    IMAGE_MALLOC_FAILURE,
    ANIMATION_MALLOC_FAILURE,
    MOVEMENT_MALLOC_FAILURE,
    ERR_MSG_MALLOC_FAILURE,
    TEXTURE_MALLOC_FAILURE
};

/* This function acts as an error logger and will take actions depending on the
 * error - mainly exit the program with EXIT_FAILURE for some errors. */
void handle_error(Errors const err_id, char const *message);

#endif // define RPG_ERROR
