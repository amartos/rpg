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
    CATCH_HANDLE(SDL_INIT_VIDEO, SDL_GetError())
    CATCH_HANDLE(WINDOW_INIT_FAILURE, SDL_GetError())
    ETRY;

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear"); // softer resizes 
    SDL_RenderSetLogicalSize(*renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
}

void apply_tiles(
        SDL_Renderer *renderer,
        Asset assets[0xFFFF],
        Map const map,
        Camera camera,
        Bool grid,
        unsigned int const level
        )
{
    unsigned int id = 0;
    SDL_Rect image_rect; init_sdl_rect(&image_rect);
    Coord positions; init_coord(&positions);
    CALC_MIN_MAX_SCROLL

    int x, y;
    for (y=miny;y<=maxy;y++)
        for (x=minx;x<=maxx;x++)
            if (
                    x >= 0 && y >= 0 &&
                    x < (int)map.maxx &&
                    y < (int)map.maxy
               )
            {
                positions.x = x; positions.y = y;
                image_rect = coord_to_isosdlrect(positions, camera);

                if (!level && grid)
                    SDL_RenderCopy(renderer, assets[0x0013].image->texture, NULL, &image_rect);

                id = map.tiles[level][y][x];
                if (id)
                {
                    image_rect.y -= level * TILES_HEIGHT; // level 1 offset
                    SDL_RenderCopy(renderer, assets[id].image->texture, NULL, &image_rect);
                }
            }
}

static void apply_characters(
        SDL_Renderer *renderer,
        Asset assets[0xFFFF],
        Map map,
        Camera camera
        )
{
    unsigned int i, min = 0x100, max = 0x100 + MAX_CHARACTERS, current_frame;
    SDL_Rect image_rect; init_sdl_rect(&image_rect);
    Coord tile_position; init_coord(&tile_position);
    Coord char_position; init_coord(&char_position);
    State state = MOVE;
    Cardinals direction = N;
    int x, y;
    CALC_MIN_MAX_SCROLL

    for (i=min;i<max;i++)
        if (assets[i].movement != NULL)
            for (y=miny;y<=maxy;y++)
                for (x=minx;x<=maxx;x++)
                    if (
                            x >= 0 && y >= 0 &&
                            x < (int)map.maxx &&
                            y < (int)map.maxy
                       )
                    {
                            char_position = assets[i].movement->position;
                            tile_position.x = x; tile_position.y = y;
                            if (is_within_tile(char_position, tile_position))
                            {
                                image_rect = coord_to_isosdlrect(char_position, camera);
                                image_rect.x += TILES_WIDTH/4; image_rect.y -= TILES_HEIGHT/4;
                                image_rect.w = scale(SPRITES_WIDTH, camera);
                                image_rect.h = scale(SPRITES_HEIGHT, camera);
                                direction = assets[i].movement->direction;
                                current_frame = assets[i].animation->current_frame;
                                SDL_RenderCopy(
                                        renderer,
                                        assets[i].image->texture,
                                        &assets[i].animation->frames[direction][state][current_frame],
                                        &image_rect
                                        );
                            }
                    }
}

void render_screen(
        SDL_Renderer *renderer,
        Asset assets[0xFFFF],
        SDL_Texture *pause_layer,
        Cursors mouse_type,
        SDL_Rect mouse_hover_rect,
        Camera camera,
        Map const map,
        Bool paused
        )
{
    unsigned int i, level = 0;
    Coord position; init_coord(&position);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF); // RGBA
    SDL_RenderDrawRect(renderer, NULL);

    apply_tiles(renderer, assets, map, camera, FALSE, level); // level 0
    apply_characters(renderer, assets, map, camera);
    for (level=1;level<MAX_LEVELS;level++)
        apply_tiles(renderer, assets, map, camera, FALSE, level); // level 0

    if (paused)
        SDL_RenderCopy(renderer, pause_layer, NULL, NULL);

    SDL_RenderCopy(renderer, assets[mouse_type].image->texture, NULL, &mouse_hover_rect);

    // show destination for characters
    for (i=0;i<0xFFFF;i++)
    {
        if (assets[i].movement != NULL && assets[i].movement->moving)
        {
            position = assets[i].movement->path[0];
            mouse_hover_rect = coord_to_isosdlrect(position, camera);
            SDL_RenderCopy(renderer, assets[VALID].image->texture, NULL, &mouse_hover_rect);
        }
    }
}
