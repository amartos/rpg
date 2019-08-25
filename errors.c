#include "errors.h"

// this is only temporary, to change in log in file
void printe(const time_t now, const int error_id, 
        const char error_def[], const char sdl_message[])
{
    printf("[%ld] Error %d (%s): %s\n", now, error_id, 
            error_def, sdl_message);
}

void logger(const int error_id, const char sdl_message[])
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
    }
}
