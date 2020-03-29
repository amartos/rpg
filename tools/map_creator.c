#include "map_creator.h"


static void print_help()
{
        printf(
"USAGE: COMMAND -n MAP_NAME -s WIDTHxHEIGHT\n \
-> MAP_NAME is of %d max characters\n \
-> WIDTH and HEIGHT are non 0 and in number of tiles\n",
            MAX_SIZE_LINE);
        exit(EXIT_SUCCESS);
}

static void get_args(int argc, char *argv[], Map *map, char map_name[MAX_SIZE_LINE])
{
    int opt = 0, maxx = 0, maxy = 0;
    while((opt = getopt(argc, argv, "n:s:")) != -1)
    {
        switch(opt)
        {
            case 'n':
                sprintf(map_name, "%s", optarg);
                break;
            case 's':
                sscanf(optarg, "%dx%d)", &maxx, &maxy);
                break;
            case 'h':
                print_help();
                break;
            default:
                break;
        }
    }

    if (maxx == 0 || maxy == 0 || map_name[0] == '\0')
        print_help();

    map->max = int_to_coord(maxx, maxy);
}

/* This function sets the currently selected tile on map.
 * For now, as there are very few tiles, all subsequent tiling for the isometric
 * z level is done manually.
 * TODO: to be adapted for multiple tiles and any levels. */
static void set_tile(Map *map, int const x, int const y, int const tile_id)
{
    map->tiles[0][y][x] = tile_id;
    if (tile_id == ID_WALL0)
    {
        map->tiles[1][y][x] = ID_WALL1;
        map->collisions[y][x] = 1;
    }
}

static void reset_tile(Map *map, int const x, int const y)
{
    for (int z=0;z<MAX_LEVELS;z++)
        map->tiles[z][y][x] = 0;
    map->collisions[y][x] = 0;
    map->cost[y][x] = 0;
    map->weather[y][x] = 0;
}

static void reset_map(Map *map)
{
    for (int y=0;y<map->max.y;y++)
        for (int x=0;x<map->max.x;x++)
            reset_tile(map, x, y);
}

/* This function change the chosen tile to be set on map. The cursor also
 * changes to reflect which tile is currently selected. */
static void change_tile(
        int *tile_id,
        Asset assets[0xFFFF],
        int const x,
        int const y,
        int const id
        )
{
    *tile_id = id;    
    set_image_position(&assets[id].image->sdlrect, x, y);
}

static void write_to_db(char const map_name[MAX_SIZE_LINE], Map const map)
{
    int x, y, z;
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
            "null", map_name, (int)map.max.x, (int)map.max.y);
    MODIFY_DB(query)

    for (y=0;y<map.max.y;y++)
        for (x=0;x<map.max.x;x++)
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
    int tile_id = ID_FLOOR, x, y, time = 0, prev_time = 0;

    SDL_Window *window; SDL_Renderer *renderer;
    init_screen(&window, &renderer);
    SDL_Event event;
    SDL_ShowCursor(SDL_DISABLE);

    Camera camera; init_camera(&camera);

    Asset assets[0xFFFF]; init_asset_array(assets); load_assets_db(renderer, assets);
    /* For now only two main tiles are available : wall or floor. Thus, this is
     * manually set.
     * TODO: set this automatically for all available tiles. */
    set_image_size(&assets[ID_FLOOR].image->sdlrect, TILES_WIDTH(camera), TILES_HEIGHT(camera));
    set_image_size(&assets[ID_WALL0].image->sdlrect, TILES_WIDTH(camera), TILES_HEIGHT(camera));

    SDL_Rect sprites_infos; init_sdl_rect(&sprites_infos);
    set_image_size(&sprites_infos, FRAMES_WIDTH, FRAMES_HEIGHT);
    SDL_Rect tiles_infos; init_sdl_rect(&tiles_infos);

    Bool quit = FALSE;

    Coord click; init_coord(&click);

    char map_name[MAX_SIZE_LINE] = {0};
    Map map; init_coord(&map.max);
    get_args(argc, argv, &map, map_name);
    init_empty_map(&map, 0, 0); // no need for maxx or maxy values

    while (!quit)
        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
                default:
                    break;
                case SDL_QUIT:
                    quit = TRUE;
                    break;
                case SDL_MOUSEMOTION:
                    set_image_position(&assets[tile_id].image->sdlrect, event.motion.x, event.motion.y);
                    break;
                case SDL_MOUSEWHEEL:
                    if (event.wheel.y > 0)
                        set_zoom(&camera, FALSE);
                    else if (event.wheel.y < 0)
                        set_zoom(&camera, TRUE);
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    click = screen_to_coord(event.button.x, event.button.y, camera);
                    coord_to_int(click, &x, &y);
                    if (!is_out_of_map(click, map))
                        switch(event.button.button)
                        {
                            case SDL_BUTTON_LEFT:
                                set_tile(&map, x, y, tile_id);
                                break;
                            case SDL_BUTTON_RIGHT:
                                reset_tile(&map, x, y);
                                break;
                        }
                    break;
                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym)
                    {
                        case SDLK_q:
                            quit = TRUE;
                            break;
                        case SDLK_a:
                            change_tile(&tile_id, assets, event.motion.x, event.motion.y, ID_FLOOR);
                            break;
                        case SDLK_z:
                            change_tile(&tile_id, assets, event.motion.x, event.motion.y, ID_WALL0);
                            break;
                        case SDLK_F5:
                            reset_map(&map);
                            break;
                        case SDLK_UP:
                            set_scroll(&camera, -1, -1);
                            break;
                        case SDLK_DOWN:
                            set_scroll(&camera, 1, 1);
                            break;
                        case SDLK_LEFT:
                            set_scroll(&camera, -1, 1);
                            break;
                        case SDLK_RIGHT:
                            set_scroll(&camera, 1, -1);
                            break;
                    }
                    break;
            }
            /* framerate limited events
             * this uses the time and prev_time ints */
            TIME
                render_screen(renderer, assets, tile_id, camera, map, FALSE, TRUE);
                SDL_RenderPresent(renderer);
            ENDTIME
        }

    write_to_db(map_name, map);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    free_assets_array(assets);
    free_map(&map);

    return EXIT_SUCCESS;
}
