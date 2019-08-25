#include "screen.h"

enum {R, G, B};

void init_screen(SDL_Surface *(*screen))
{
    TRY
    {
        if (SDL_Init(SDL_INIT_VIDEO))
            THROW(SCREEN_INIT_FAILURE);
        *screen = SDL_SetVideoMode(
                SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, 
                SDL_RESIZABLE | SDL_HWSURFACE | SDL_DOUBLEBUF
                );
        if (screen == NULL)
            THROW(VIDEO_MODE_LOAD_FAILURE);
    }
    CATCH(SCREEN_INIT_FAILURE)
    {
        logger(SCREEN_INIT_FAILURE, SDL_GetError());
        exit(EXIT_FAILURE);
    }
    CATCH(VIDEO_MODE_LOAD_FAILURE)
    {
        logger(SCREEN_INIT_FAILURE, SDL_GetError());
        exit(EXIT_FAILURE);
    }
    ETRY;
}


void set_BG_color(SDL_Surface *(*screen), const int rgb[3])
{
    SDL_FillRect(*screen, NULL, SDL_MapRGB((*screen)->format, rgb[R], rgb[G], rgb[B])); 
}
