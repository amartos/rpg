#include "errors.h"


// this is only temporary, to change in log in file
static void printe(
        time_t const now,
        unsigned int const error_id,
        char const error_def[],
        char const sdl_message[]
        )
{
    printf("[%ld] Error %d (%s): %s\n",
            now,
            error_id,
            error_def, sdl_message
            );
}

void logger(Errors const error_id, char const sdl_message[])
{
    time_t now;
    time(&now);
    switch(error_id)
    {
        case SCREEN_INIT_FAILURE:
            printe(now, error_id, "SDL init failed", sdl_message);
            break;
        case VIDEO_MODE_LOAD_FAILURE:
            printe(now, error_id, "video mode loading failed", sdl_message);
            break;
        case SPRITE_LOAD_FAILURE:
            printe(now, error_id, "sprite load failure", sdl_message);
            break;
        case FLIP_SCREEN_FAILURE:
            printe(now, error_id, "flip screen failure", sdl_message);
            break;
        case CHARACTER_NOT_ANIMATED:
            printe(now, error_id, "character not animated", sdl_message);
            break;
        case CHARACTER_NOT_MOVING:
            printe(now, error_id, "character not moving", sdl_message);
            break;
        case MAP_FILE_LOADING_FAILURE:
            printe(now, error_id, "map file loading failure", sdl_message);
            break;
        case CONVERTER_FILE_LOADING_FAILURE:
            printe(now, error_id, "converter file loading failure", sdl_message);
            break;
        case MAP_MALLOC_FAILURE:
            printe(now, error_id, "map memory allocation failure", sdl_message);
            break;
        case PATHFIND_MALLOC_FAILURE:
            printe(now, error_id, "pathfind memory allocation failure", sdl_message);
            break;
    }
}
