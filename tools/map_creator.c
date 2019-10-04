#include "map_creator.h"


static void map_creator_render_screen(
        SDL_Renderer *renderer,
        Asset assets[0xFFFF],
        unsigned int tile_id,
        Coord center,
        Map const map,
        Camera camera
        )
{
    Coord position; init_coord(&position);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF); // RGBA
    SDL_RenderDrawRect(renderer, NULL);

    unsigned int level;
    for (level=0;level<MAX_LEVELS;level++)
        apply_tiles(renderer, assets, map, camera, TRUE, level);

    SDL_Rect mouse_hover_rect; init_sdl_rect(&mouse_hover_rect);
    mouse_hover_rect = coord_to_isosdlrect(center, camera);
    if (tile_id)
        SDL_RenderCopy(renderer, assets[tile_id].image->texture, NULL, &mouse_hover_rect);
    else
        SDL_RenderCopy(renderer, assets[HOVER].image->texture, NULL, &mouse_hover_rect);

    SDL_RenderPresent(renderer);
}

static void write_to_db(char const map_name[MAX_SIZE_LINE], Map const map)
{
    unsigned int x, y, z;
    char query[MAX_SIZE_LINE] = {0};
    char level_value[MAX_SIZE_LINE] = {0};
    INIT_DB

    sprintf(query, "create table %s( \
        x int not null, y int not null, \
        collision int not null, cost int not null, \
        weather int not null", map_name);
    ZLOOP(0, ", z%d hex not null", z)
    strcat(query, ", foreign key (z0");
    ZLOOP(1, ", z%d", z)
    strcat(query, ") references images(id");
    ZLOOP(1, ", id", NULL)
    strcat(query, ") on update cascade on delete cascade, \
                    primary key (x,y));");
    MODIFY_DB(query)

    sprintf(query,
            "insert into maps values(%s, '%s', %d, %d);",
            "null", map_name, map.maxx, map.maxy);
    MODIFY_DB(query)

    for (y=0;y<map.maxy;y++)
        for (x=0;x<map.maxx;x++)
        {
            sprintf(query, "insert into %s values(%d, %d, %d, %d, %d",
                    map_name, x, y, map.collisions[y][x], map.cost[y][x], map.weather[y][x]);
            for (z=0;z<MAX_LEVELS;z++)
            {
                sprintf(level_value, ", %d", map.tiles[z][y][x]);
                strcat(query, level_value);
            }
            strcat(query, ");");
            MODIFY_DB(query)
        }
    CLOSE_DB
}

int main(int argc, char *argv[])
{
    unsigned int tile_id = 0, x, y;

    // SDL vars init
    SDL_Window *window; SDL_Renderer *renderer;
    init_screen(&window, &renderer);

    Camera camera; init_camera(&camera);
    camera.scroll.x = (SCREEN_WIDTH/2) / TILES_WIDTH;
    camera.scroll.y = (SCREEN_HEIGHT/2) / TILES_HEIGHT;

    Asset assets[0xFFFF]; init_asset_array(assets); load_assets_db(renderer, assets);

    SDL_Rect sprites_infos; init_sdl_rect(&sprites_infos);
    sprites_infos.w = SPRITES_WIDTH; sprites_infos.h = SPRITES_HEIGHT;
    SDL_Rect tiles_infos; init_sdl_rect(&tiles_infos);

    SDL_Event event;

    // custom structs init
    Bool done = FALSE;

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

    SDL_ShowCursor(SDL_DISABLE);

    // main loop
    while (!done)
    {
        SDL_WaitEvent(&event);
        switch(event.type)
        {
            case SDL_QUIT:
                done = TRUE;
                break;
            case SDL_MOUSEMOTION:
                center = event_to_coord(event.motion.x, event.motion.y, camera);
                break;
            case SDL_MOUSEBUTTONDOWN:
                center = event_to_coord(event.button.x, event.button.y, camera);
                if (!is_out_of_map(center, max_coord))
                {
                    x = center.x; y = center.y;
                    switch(event.button.button)
                    {
                        case SDL_BUTTON_LEFT:
                            map.tiles[0][y][x] = tile_id;
                            if (tile_id == ID_WALL0)
                            {
                                map.tiles[1][y][x] = ID_WALL1;
                                map.collisions[y][x] = 1;
                            }
                            break;
                        case SDL_BUTTON_RIGHT:
                            map.tiles[0][y][x] = 0;
                            map.tiles[1][y][x] = 0;
                            map.collisions[y][x] = 0;
                            break;
                    }
                }
                break;
            case SDL_MOUSEWHEEL:
                if (event.wheel.y > 0 && camera.scale < 1)
                    camera.scale *= 2;
                else if (event.wheel.y < 0 && camera.scale > 0.5)
                    camera.scale *= 0.5;
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
                        for (y=0;y<maxy;y++)
                            for (x=0;x<maxx;x++)
                            {
                                map.tiles[0][y][x] = 0;
                                map.tiles[1][y][x] = 0;
                                map.collisions[y][x] = 0;
                                map.cost[y][x] = 0;
                                map.weather[y][x] = 0;
                            }
                        break;
                    case SDLK_UP:
                        camera.scroll.x -= 1;
                        camera.scroll.y -= 1;
                        break;
                    case SDLK_DOWN:
                        camera.scroll.x += 1;
                        camera.scroll.y += 1;
                        break;
                    case SDLK_LEFT:
                        camera.scroll.x -= 1;
                        camera.scroll.y += 1;
                        break;
                    case SDLK_RIGHT:
                        camera.scroll.x += 1;
                        camera.scroll.y -= 1;
                        break;
                }
                break;
        }

        // Rendering
        map_creator_render_screen(renderer, assets, tile_id, center, map, camera);
    }

    write_to_db(map_name, map);

    // free all freeables
    free_assets_array(assets);
    free_map(&map);
    SDL_Quit();

    return EXIT_SUCCESS;
}
