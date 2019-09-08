#include "screen.h"


void init_screen(SDL_Window **window, SDL_Renderer **renderer)
{
    TRY
    {
        if (SDL_Init(SDL_INIT_VIDEO))
            THROW(VIDEO_INIT_FAILURE);
        if (
                SDL_CreateWindowAndRenderer(
                0, 0, // to replace with FULL_SCREEN_DESKTOP
                SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED,
                window, renderer
                )
           )
            THROW(WINDOW_INIT_FAILURE);
    }
    CATCH(SDL_INIT_VIDEO)
    {
        logger(VIDEO_INIT_FAILURE, SDL_GetError());
        exit(EXIT_FAILURE);
    }
    CATCH(WINDOW_INIT_FAILURE)
    {
        logger(WINDOW_INIT_FAILURE, SDL_GetError());
        exit(EXIT_FAILURE);
    }
    ETRY;

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear"); // softer resizes 
    SDL_RenderSetLogicalSize(*renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
}

void apply_tiles(SDL_Renderer **renderer, MapType const type, Map const map, SDL_Texture* tiles[0xFFFF])
{
    unsigned int i, j, id = 0;
    SDL_Rect tiles_positions;
    tiles_positions.w = TILES_WIDTH*2; tiles_positions.h = TILES_HEIGHT*2;
    Coord positions; init_coord(&positions);

    for (i=0;i<map.maxx;i++)
        for (j=0;j<map.maxy;j++)
        {
            positions.x = i * TILES_WIDTH;
            positions.y = j * TILES_HEIGHT;
            positions = cartesian_to_isometric(positions);
            tiles_positions.x = positions.x;
            tiles_positions.y = positions.y; // offset
            id = map.schematics[type][i][j];
            SDL_RenderCopy(*renderer, tiles[id], NULL, &tiles_positions);
        }
}

// void make_check_board(SDL_Renderer **renderer, unsigned int const x, unsigned int const y) // max tiles
// {
//     unsigned int i, j;
//     unsigned int screen_black_color[3] = {0xD3, 0xD3, 0xD3};
//     SDL_Rect infos;
//     Coord positions; init_coord(&positions);
//     infos.w = TILES_WIDTH; infos.h = TILES_HEIGHT;
//     SDL_FillRect(*screen, NULL, SDL_MapRGB((*screen)->format, 0xff, 0xff, 0xff));
//     for (i=0;i<x;i++)
//         for (j=0;j<y;j++)
//         {
//             if ((i-(j%2))%2)
//             {
//                 positions.x = i * TILES_WIDTH;
//                 positions.y = j * TILES_HEIGHT;
//                 positions = cartesian_to_isometric(positions);
//                 infos.x = positions.x;
//                 infos.y = positions.y;
//                 set_BG_color(screen, &infos, screen_black_color);
//             }
//         }
// }
