#ifndef RPG_ERRORS
#define RPG_ERRORS

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// prototypes
void logger(const int error_id, const char sdl_message[]);

// custom errors
#define SCREEN_INIT_FAILURE 1
#define VIDEO_MODE_LOAD_FAILURE 2
#define SPRITE_LOAD_FAILURE 3


#endif // define RPG_ERROR
