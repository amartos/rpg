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
        SDL_Renderer *renderer,
        Map const map,
        AnimatedObject objects[],
        unsigned int max_objects,
        Image images[0xFFFF],
        Coord scroll
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
    int minx = scroll.x - 1; // show even the tile that is partly out of screen
    int maxx = scroll.x + SCREEN_WIDTH/TILES_WIDTH;
    int miny = scroll.y - 1; // show even the tile that is partly out of screen
    int maxy = scroll.y + SCREEN_HEIGHT/TILES_HEIGHT;

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
                        SDL_RenderCopy(renderer, images[id].texture, NULL, &image_rect);
                    }
                    if (level == 1)
                    {
                        for (i=0;i<max_objects;i++)
                        {
                            char_position = objects[i].movement.position;
                            tile_position.x = x; tile_position.y = y;
                            if (is_within_tile(char_position, tile_position))
                            {
                                char_position.x -= minx; char_position.y -= miny;
                                image_rect = coord_to_isosdlrect(char_position);
                                image_rect.x += TILES_WIDTH/4; image_rect.y -= TILES_HEIGHT/4;
                                image_rect.w = SPRITES_WIDTH; image_rect.h = SPRITES_HEIGHT;
                                direction = objects[i].movement.direction;
                                current_frame = objects[i].animation.current_frame;
                                image_id = 0x100 + i;
                                SDL_RenderCopy(
                                        renderer,
                                        images[image_id].texture,
                                        &objects[i].animation.frames[direction][state][current_frame],
                                        &image_rect
                                        );
                            }

                        }
                    }
                }
}

void render_screen(
        SDL_Renderer *renderer,
        AnimatedObject objects[],
        unsigned int max_objects,
        Image images[0xFFFF],
        SDL_Texture *pause_layer,
        Cursors mouse_type,
        SDL_Rect mouse_hover_rect,
        Coord scroll,
        Map const map,
        Bool paused
        )
{
    unsigned int i;
    Coord position; init_coord(&position);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF); // RGBA
    SDL_RenderDrawRect(renderer, NULL);

    apply_tiles(renderer, map, objects, 4, images, scroll);

    if (paused)
        SDL_RenderCopy(renderer, pause_layer, NULL, NULL);

    SDL_RenderCopy(renderer, images[mouse_type].texture, NULL, &mouse_hover_rect);

    for (i=0;i<max_objects;i++)
    {
        if (objects[i].movement.moving)
        {
            position = objects[i].movement.path[0];
            position.x -= scroll.x; position.y -= scroll.y;
            mouse_hover_rect = coord_to_isosdlrect(position);
            SDL_RenderCopy(renderer, images[VALID].texture, NULL, &mouse_hover_rect);
        }
    }
}
