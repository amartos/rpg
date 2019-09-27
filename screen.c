#include "screen.h"


void init_sdl_rect(SDL_Rect *rect)
{
    rect->x = 0; rect->y = 0;
    rect->w = 0; rect->h = 0;
}

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

void apply_tiles(
        SDL_Renderer **renderer,
        Map const map,
        Character all_characters[MAX_CHARACTERS],
        Image images[0xFFFF],
        int const xscroll, int const yscroll
        )
{
    unsigned int id = 0, i, image_id;
    SDL_Rect image_rect; init_sdl_rect(&image_rect);
    Coord positions; init_coord(&positions);
    Coord tile_position; init_coord(&tile_position);
    Coord char_position; init_coord(&char_position);
    State state = MOVE;
    Cardinals direction = N;
    unsigned int current_frame = 0, level = 0;
    int x, y;
    int minx = xscroll - 1; // show even the tile that is partly out of screen
    int maxx = xscroll + SCREEN_WIDTH/TILES_WIDTH;
    int miny = yscroll - 1; // show even the tile that is partly out of screen
    int maxy = yscroll + SCREEN_HEIGHT/TILES_HEIGHT;

    for (level=0;level<MAX_LEVELS;level++)
        for (y=miny;y<=maxy;y++)
            for (x=minx;x<=maxx;x++)
                if (
                        x >= 0 && y >= 0 &&
                        x < (int)map.maxx &&
                        y < (int)map.maxy
                   )
                {
                    positions.x = x - minx; positions.y = y - miny;
                    image_rect = coord_to_isosdlrect(positions);
                    id = map.tiles[level][y][x];
                    if (id)
                    {
                        image_rect.y -= level * TILES_HEIGHT; // level 1 offset
                        image_rect.w = TILES_WIDTH; image_rect.h = TILES_HEIGHT;
                        SDL_RenderCopy(*renderer, images[id].texture, NULL, &image_rect);
                    }
                    if (level == 1)
                    {
                        for (i=0;i<MAX_CHARACTERS;i++)
                        {
                            char_position = all_characters[i].movement.position;
                            tile_position.x = x; tile_position.y = y;
                            if (is_within_tile(char_position, tile_position))
                            {
                                char_position.x -= minx; char_position.y -= miny;
                                image_rect = coord_to_isosdlrect(char_position);
                                image_rect.x += TILES_WIDTH/4; image_rect.y -= TILES_HEIGHT/4;
                                image_rect.w = SPRITES_WIDTH; image_rect.h = SPRITES_HEIGHT;
                                direction = all_characters[i].movement.direction;
                                current_frame = all_characters[i].on_screen.current_frame;
                                image_id = 0x100 + i;
                                SDL_RenderCopy(
                                        *renderer,
                                        images[image_id].texture,
                                        &all_characters[i].on_screen.frames[direction][state][current_frame],
                                        &image_rect
                                        );
                            }

                        }
                    }
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
