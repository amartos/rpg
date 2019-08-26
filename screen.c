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

void apply_background(SDL_Surface *(*screen), const Map map, SDL_Surface *tile)
{
    int i, j;
    SDL_Rect tiles_positions;
    for (i=0;i<map.infos.x;i++)
    {
        for (j=0;j<map.infos.y;j++)
        {
            tiles_positions.x = i * TILES_WIDTH;
            tiles_positions.y = j * TILES_HEIGHT;
            if (map.tiles[i][j])
                SDL_BlitSurface(tile, NULL, *screen, &tiles_positions);
        }
    }
}
