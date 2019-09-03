#include "screen.h"


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

void set_BG_color(SDL_Surface *(*screen), SDL_Rect* const tile, unsigned int* const rgb)
{
    SDL_FillRect(*screen, tile, SDL_MapRGB((*screen)->format, rgb[R], rgb[G], rgb[B])); 
}

void apply_tiles(SDL_Surface *(*screen), MapType const type, Map const map, SDL_Surface* const tile)
{
    unsigned int i, j;
    SDL_Rect tiles_positions;
    for (i=0;i<map.x_tiles;i++)
        for (j=0;j<map.y_tiles;j++)
        {
            tiles_positions.x = i * TILES_WIDTH;
            tiles_positions.y = j * TILES_HEIGHT;
            if (map.schematics[type][i][j])
                SDL_BlitSurface(tile, NULL, *screen, &tiles_positions);
        }
}

void make_check_board(SDL_Surface *(*screen), unsigned int const x, unsigned int const y) // max tiles
{
    unsigned int i, j;
    unsigned int screen_black_color[3] = {0xD3, 0xD3, 0xD3};
    SDL_Rect infos;
    infos.w = TILES_WIDTH; infos.h = TILES_HEIGHT;
    for (i=0;i<x;i++)
        for (j=0;j<y;j++)
        {
            if ((i-(j%2))%2)
            {
                infos.x = i * TILES_WIDTH;
                infos.y = j * TILES_HEIGHT;
                set_BG_color(screen, &infos, screen_black_color);
            }
        }
}
