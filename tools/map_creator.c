#include "map_creator.h"


int main(int argc, char *argv[])
{
    unsigned int i = 0, j = 0, t = 0, tile_id = 0;

    // SDL vars init
    SDL_Window *window; SDL_Renderer *renderer;
    init_screen(&window, &renderer);

    SDL_Texture* tiles[0xFFFF];
    for (i=0;i<0xFFFF;i++)
        tiles[i] = NULL;

    SDL_Surface *floor_surface = IMG_Load("assets/tiles/tile.png");
    SDL_Surface *wall_surface = IMG_Load("assets/tiles/wall.png");
    SDL_Surface *wall1_surface = IMG_Load("assets/tiles/wall1.png");

    SDL_Rect sprites_infos;
    sprites_infos.x = 0; sprites_infos.y = 0;
    sprites_infos.w = SPRITES_WIDTH; sprites_infos.h = SPRITES_HEIGHT;
    SDL_Rect tiles_infos;
    tiles_infos.x = 0; tiles_infos.y = 0;
    tiles_infos.w = TILES_WIDTH*2; tiles_infos.h = TILES_HEIGHT*2;

    SDL_Event event;

    // custom structs init
    Bool done = FALSE;

    Coord origin; init_coord(&origin);
    Coord vertical; init_coord(&vertical);
    Coord horizontal; init_coord(&horizontal);
    Coord max_coord; init_coord(&max_coord);
    Coord center; init_coord(&center);
    Coord isometrified; init_coord(&isometrified);

    char map_name[MAX_SIZE_LINE] = {0};
    unsigned int maxx = 20, maxy = 20;
    printf("map name (no spaces, %d max characters): ", MAX_SIZE_LINE); scanf("%s", map_name);
    printf("max tiles [x y]: "); scanf("%d %d", &maxx, &maxy);
    Map map; init_empty_map(&map, maxx, maxy);
    max_coord.x = map.maxx;
    max_coord.y = map.maxy;

    floor_surface = SDL_ConvertSurfaceFormat(floor_surface, SDL_PIXELFORMAT_RGBA8888, 0);
    wall_surface = SDL_ConvertSurfaceFormat(wall_surface, SDL_PIXELFORMAT_RGBA8888, 0);
    wall1_surface = SDL_ConvertSurfaceFormat(wall1_surface, SDL_PIXELFORMAT_RGBA8888, 0);

    SDL_Texture *floor = SDL_CreateTextureFromSurface(renderer, floor_surface);
    SDL_Texture *wall = SDL_CreateTextureFromSurface(renderer, wall_surface);
    SDL_Texture *wall1 = SDL_CreateTextureFromSurface(renderer, wall1_surface);

    tiles[0x0100] = floor;
    tiles[0x0101] = wall;
    tiles[0x0102] = wall1;

    SDL_Surface* mouse_hover_surfaces[INVALID+1];
    for (i=0;i<=INVALID;i++)
    {
        mouse_hover_surfaces[i] = IMG_Load("assets/mouse/cursors.png");
        if (mouse_hover_surfaces[i] == NULL)
            exit(EXIT_FAILURE);
    }

    SDL_Color cursors_border[INVALID+1];
    cursors_border[EMPTY].r = 0x3d; cursors_border[EMPTY].g = 0x3d; cursors_border[EMPTY].b = 0x3d; cursors_border[EMPTY].a = 255;
    cursors_border[HOVER].r = 0; cursors_border[HOVER].g = 255; cursors_border[HOVER].b = 0; cursors_border[HOVER].a = 255;
    cursors_border[VALID].r = 0; cursors_border[VALID].g = 0; cursors_border[VALID].b = 255; cursors_border[VALID].a = 255;
    cursors_border[INVALID].r = 255; cursors_border[INVALID].g = 0; cursors_border[INVALID].b = 0; cursors_border[INVALID].a = 255;

    SDL_Color cursors_inside[INVALID+1];
    cursors_inside[EMPTY].r = 0xe4; cursors_inside[EMPTY].g = 0xe4; cursors_inside[EMPTY].b = 0xe4; cursors_inside[EMPTY].a = 255;
    cursors_inside[HOVER].r = 0; cursors_inside[HOVER].g = 255; cursors_inside[HOVER].b = 0; cursors_inside[HOVER].a = 76;
    cursors_inside[VALID].r = 0; cursors_inside[VALID].g = 0; cursors_inside[VALID].b = 255; cursors_inside[VALID].a = 76;
    cursors_inside[INVALID].r = 255; cursors_inside[INVALID].g = 0; cursors_inside[INVALID].b = 0; cursors_inside[INVALID].a = 76;

    SDL_Texture* mouse[INVALID+1];
    mouse[EMPTY] = NULL;
    for (i=1;i<=INVALID;i++)
    {
        set_color(mouse_hover_surfaces[i], cursors_border[EMPTY], cursors_border[i]);
        set_color(mouse_hover_surfaces[i], cursors_inside[EMPTY], cursors_inside[i]);
        mouse_hover_surfaces[i] = SDL_ConvertSurfaceFormat(mouse_hover_surfaces[i], SDL_PIXELFORMAT_RGBA8888, 0);
        mouse[i] = SDL_CreateTextureFromSurface(renderer, mouse_hover_surfaces[i]);
    }
    SDL_Rect mouse_hover_rect;
    mouse_hover_rect.x = 0; mouse_hover_rect.y = 0;
    mouse_hover_rect.w = TILES_WIDTH*2; mouse_hover_rect.h = TILES_HEIGHT*2;

    SDL_FreeSurface(floor_surface);
    SDL_FreeSurface(wall_surface);
    SDL_FreeSurface(wall1_surface);
    for (i=0;i<=INVALID;i++)
        SDL_FreeSurface(mouse_hover_surfaces[i]);

    // main loop
    while (!done)
    {
        // Rendering
        SDL_RenderClear(renderer); 

        apply_tiles(&renderer, BACKGROUND, map, tiles);
        apply_tiles(&renderer, FOREGROUND, map, tiles);

        SDL_WaitEvent(&event);
        switch(event.type)
        {
            case SDL_QUIT:
                done = TRUE;
                break;
            case SDL_MOUSEMOTION:
                center.x = event.motion.x;
                center.y = event.motion.y;
                center = isometric_to_cartesian(center);
                // round coord to current tile, not *exact* click position
                round_coord(&center);
                if (!is_out_of_map(center, max_coord))
                {
                    isometrified = cartesian_to_isometric(center);
                    mouse_hover_rect.x = isometrified.x;
                    mouse_hover_rect.y = isometrified.y;
                    if (tile_id)
                        SDL_RenderCopy(renderer, tiles[tile_id], NULL, &mouse_hover_rect);
                    else
                        SDL_RenderCopy(renderer, mouse[HOVER], NULL, &mouse_hover_rect);
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                center.x = event.button.x;
                center.y = event.button.y;
                center = isometric_to_cartesian(center);
                // round coord to current tile, not *exact* click position
                round_coord(&center);
                if (!is_out_of_map(center, max_coord))
                {
                    isometrified = cartesian_to_isometric(center);
                    pixels_to_unit(&center);
                    mouse_hover_rect.x = isometrified.x;
                    mouse_hover_rect.y = isometrified.y;
                    switch(event.button.button)
                    {
                        case SDL_BUTTON_LEFT:
                            SDL_RenderCopy(renderer, mouse[VALID], NULL, &mouse_hover_rect);
                            map.schematics[BACKGROUND][center.x][center.y] = tile_id;
                            if (tile_id == ID_WALL0)
                            {
                                map.schematics[FOREGROUND][center.x][center.y] = ID_WALL1;
                                map.schematics[COLLISIONS][center.x][center.y] = 1;
                            }
                            break;
                        case SDL_BUTTON_RIGHT:
                            SDL_RenderCopy(renderer, mouse[INVALID], NULL, &mouse_hover_rect);
                            map.schematics[BACKGROUND][center.x][center.y] = 0;
                            map.schematics[FOREGROUND][center.x][center.y] = 0;
                            map.schematics[COLLISIONS][center.x][center.y] = 0;
                            break;
                    }
                }
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                    case SDLK_a:
                        tile_id = ID_FLOOR;
                        break;
                    case SDLK_z:
                        tile_id = ID_WALL0;
                        break;
                    case SDLK_e:
                        tile_id = 0;
                        break;
                    case SDLK_F5:
                        tile_id = 0;
                        for (t=BACKGROUND;t<=WEATHER;t++)
                            for (i=0;i<maxx;i++)
                                for (j=0;j<maxy;j++)
                                    map.schematics[t][i][j] = 0;
                        break;
                }
                break;
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        for (i=0;i<=maxx;i++)
            for (j=0;j<=maxy;j++)
            {
                center.x = i * TILES_WIDTH + TILES_WIDTH; center.y = j * TILES_HEIGHT;

                origin = cartesian_to_isometric(center);

                vertical.x = center.x; vertical.y = center.y + TILES_HEIGHT;
                vertical = cartesian_to_isometric(vertical);

                horizontal.x = center.x + TILES_WIDTH; horizontal.y = center.y;
                horizontal = cartesian_to_isometric(horizontal);

                if (i < maxx)
                    SDL_RenderDrawLine(renderer, origin.x, origin.y, horizontal.x, horizontal.y);
                if (j<maxy)
                    SDL_RenderDrawLine(renderer, origin.x, origin.y, vertical.x, vertical.y);

            }
        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF); // RGBA

        SDL_RenderPresent(renderer);
    }

    FILE *map_file = NULL;// FILE *converter_file = NULL;
    TRY
    {
        map_file = fopen(map_name, "a");
        if (map_file == NULL)
            THROW(MAP_FILE_LOADING_FAILURE);
    }
    CATCH(MAP_FILE_LOADING_FAILURE)
    {
        char error_msg[MAX_SIZE_LINE+50] = {0};
        sprintf(error_msg, "could not open map file at %s", map_name);
        logger(MAP_FILE_LOADING_FAILURE, error_msg);
        exit(EXIT_FAILURE);
    }
    ETRY;

    char line[MAX_SIZE_LINE] = {0};
    for (t=0;t<=WEATHER;t++)
    {
        switch(t)
        {
            case BACKGROUND:
                fprintf(map_file, "# %s\n\n", "background");
                break;
            case FOREGROUND:
                fprintf(map_file, "# %s\n\n", "foreground");
                break;
            case COLLISIONS:
                fprintf(map_file, "# %s\n\n", "collisions");
                break;
            case COST:
                fprintf(map_file, "# %s\n\n", "movement cost");
                break;
            case WEATHER:
                fprintf(map_file, "# %s\n\n", "weather");
                break;
        }
        for (j=0;j<maxy;j++)
        {
            for (i=0;i<maxx;i++) // inverted to get per line
            {
                switch(t)
                {
                    case BACKGROUND:
                    case WEATHER:
                    case FOREGROUND:
                        sprintf(line+(i*5), "%04X ", map.schematics[t][i][j]);
                        break;
                    case COST:
                    case COLLISIONS:
                        sprintf(line+(i*2), "%d ", map.schematics[t][i][j]);
                        break;
                }
            }
            fprintf(map_file, "%s\n", line);
        }
        fprintf(map_file, "\n");
    }
    fclose(map_file);

    free_map(&map);
    SDL_Quit();
    return 0;
}
