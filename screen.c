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

void apply_tiles(SDL_Surface *(*screen), MapType const type, Map const map, SDL_Surface* tiles[0xFFFF])
{
    unsigned int i, j, id = 0;
    SDL_Rect tiles_positions;
    Coord positions; init_coord(&positions);

    for (i=0;i<map.x_tiles;i++)
        for (j=0;j<map.y_tiles;j++)
        {
            positions.x = i * TILES_WIDTH;
            positions.y = j * TILES_HEIGHT;
            positions = cartesian_to_isometric(positions);
            tiles_positions.x = positions.x;
            tiles_positions.y = positions.y - type * TILES_HEIGHT; // offset
            id = map.schematics[type][i][j];
            SDL_BlitSurface(tiles[id], NULL, *screen, &tiles_positions);
        }
}

void make_check_board(SDL_Surface *(*screen), unsigned int const x, unsigned int const y) // max tiles
{
    unsigned int i, j;
    unsigned int screen_black_color[3] = {0xD3, 0xD3, 0xD3};
    SDL_Rect infos;
    Coord positions; init_coord(&positions);
    infos.w = TILES_WIDTH; infos.h = TILES_HEIGHT;
    SDL_FillRect(*screen, NULL, SDL_MapRGB((*screen)->format, 0xff, 0xff, 0xff));
    for (i=0;i<x;i++)
        for (j=0;j<y;j++)
        {
            if ((i-(j%2))%2)
            {
                positions.x = i * TILES_WIDTH;
                positions.y = j * TILES_HEIGHT;
                positions = cartesian_to_isometric(positions);
                infos.x = positions.x;
                infos.y = positions.y;
                set_BG_color(screen, &infos, screen_black_color);
            }
        }
}
