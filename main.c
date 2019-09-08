#include "main.h"


static void check_character_frame(OnScreen *on_screen, unsigned int const time)
{
    if (time - on_screen->time > on_screen->framerate)
    {
        on_screen->current_frame++;
        if (on_screen->current_frame >= on_screen->total_frames)
            on_screen->current_frame = 0;
        on_screen->time = time;
    }
}

static void fire_movement(Movement *movement, MovementType const movement_type)
{
    movement->moving = TRUE;
    movement->movement_type = movement_type;
}

static void change_formation(Character characters[MAX_CHARACTERS], Deployment formation)
{
    unsigned int i;
    for (i=0;i<MAX_CHARACTERS;i++)
    {
        characters[i].movement.formation = formation;
        fire_movement(&characters[i].movement, PATH);
    }
}

int main(int argc, char *argv[])
{
    unsigned int i = 0, j = 0, c = 0, id = 0, current_frame = 0;
    unsigned int screen_bg_color[3] = {0xFF, 0xFF, 0xFF};
    unsigned int time = 0, prev_time = 0;

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
    Bool paused = FALSE;

    MovementType movement_type = WALK;
    Cardinals direction = S;

    Character all_characters[MAX_CHARACTERS];
    State state = MOVE;

    Coord coord2; init_coord(&coord2);
    Coord coord; init_coord(&coord);
    Coord max_coord; init_coord(&max_coord);
    Coord center; init_coord(&center);
    Coord isometrified; init_coord(&isometrified);

    Map test_map; init_map(&test_map, "assets/maps/test_map2");
    max_coord.x = test_map.x_tiles;
    max_coord.y = test_map.y_tiles;

    // load characters
    center.x = TILES_WIDTH * 4;
    center.y = TILES_HEIGHT * 4;
    for (i=0;i<MAX_CHARACTERS;i++)
        init_character(&renderer, &all_characters[i], i, center, SQUARE);

    floor_surface = SDL_ConvertSurfaceFormat(floor_surface, SDL_PIXELFORMAT_RGBA8888, 0);
    wall_surface = SDL_ConvertSurfaceFormat(wall_surface, SDL_PIXELFORMAT_RGBA8888, 0);
    wall1_surface = SDL_ConvertSurfaceFormat(wall1_surface, SDL_PIXELFORMAT_RGBA8888, 0);

    SDL_Texture *floor = SDL_CreateTextureFromSurface(renderer, floor_surface);
    SDL_Texture *wall = SDL_CreateTextureFromSurface(renderer, wall_surface);
    SDL_Texture *wall1 = SDL_CreateTextureFromSurface(renderer, wall1_surface);

    tiles[0x0100] = floor;
    tiles[0x0101] = wall;
    tiles[0x0102] = wall1;

    SDL_Surface *red_rect_surface = SDL_CreateRGBSurface(0, TILES_WIDTH, TILES_HEIGHT, SCREEN_BPP, 0x0, 0, 0, 0x0);
    SDL_FillRect(red_rect_surface, NULL, SDL_MapRGB(red_rect_surface->format, 255, 0, 0));
    red_rect_surface = SDL_ConvertSurfaceFormat(red_rect_surface, SDL_PIXELFORMAT_RGBA8888, 0);
    SDL_Texture *red_rect = SDL_CreateTextureFromSurface(renderer, red_rect_surface);

    SDL_FreeSurface(floor_surface);
    SDL_FreeSurface(wall_surface);
    SDL_FreeSurface(wall1_surface);
    SDL_FreeSurface(red_rect_surface);

    // main loop
    while (!done)
    {
        time = SDL_GetTicks();
        if (time - prev_time > FRAMERATE)
        {
            prev_time = time;
            SDL_PollEvent(&event);
            switch(event.type)
            {
                case SDL_QUIT:
                    done = TRUE;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    center.x = event.button.x;
                    center.y = event.button.y;
                    center = isometric_to_cartesian(center);
                    // round coord to current tile, not *exact* click position
                    round_coord(&center);
                    for (i=0;i<MAX_CHARACTERS;i++)
                    {
                        coord = center;
                        deploy(
                                &coord,
                                determine_direction(all_characters[i].movement.position, center),
                                all_characters[i].movement.formation, i
                                );
                        if (
                                !is_same_coord(coord, all_characters[i].movement.position) &&
                                !is_out_of_map(coord, max_coord) &&
                                !is_colliding(coord, test_map.schematics[COLLISIONS], TRUE)
                            )
                        {
                            all_characters[i].movement.current_node = 0;
                            all_characters[i].movement.path[0] = coord;
                            switch(event.button.button)
                            {
                                case SDL_BUTTON_LEFT:
                                    fire_movement(&all_characters[i].movement, PATH);
                                    break;
                                case SDL_BUTTON_RIGHT:
                                    fire_movement(&all_characters[i].movement, TELEPORT);
                                    break;
                            }
                        }
                        else
                        {
                            all_characters[i].movement.current_node = 0;
                            reset_coord(&all_characters[i].movement.path[0]);
                            all_characters[i].movement.movement_type = WALK;
                            all_characters[i].movement.moving = FALSE;
                        }
                    }
                    break;
                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym)
                    {
                        case SDLK_SPACE:
                            paused = !paused;
                            break;
                        case SDLK_a:
                            change_formation(all_characters, LINE);
                            break;
                        case SDLK_z:
                            change_formation(all_characters, SQUARE);
                            break;
                        case SDLK_e:
                            change_formation(all_characters, TRIANGLE);
                            break;
                        case SDLK_r:
                            change_formation(all_characters, CIRCLE);
                            break;
                        case SDLK_UP:
                            break;
                        case SDLK_DOWN:
                            break;
                        case SDLK_LEFT:
                            break;
                        case SDLK_RIGHT:
                            break;
                    }
                    break;
            }

            // Check char actions
            for (i=0;i<MAX_CHARACTERS;i++)
            {
                if (all_characters[i].movement.moving && !paused)
                {
                    move(
                            &all_characters[i].movement,
                            max_coord,
                            test_map.schematics[COLLISIONS],
                            test_map.schematics[COST]
                            );
                    check_character_frame(&(all_characters[i].on_screen), time);
                }
            }

            // Rendering
            SDL_RenderClear(renderer); 

            SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF); // RGBA
            SDL_RenderDrawRect(renderer, NULL);

            apply_tiles(&renderer, BACKGROUND, test_map, tiles);
            // inverted to draw isometric properly
            for (j=0;j<max_coord.y;j++)
                for (i=0;i<max_coord.x;i++)
                {
                    id = test_map.schematics[FOREGROUND][i][j];
                    if (id)
                    {
                        coord.x = i * TILES_WIDTH;
                        coord.y = j * TILES_HEIGHT;
                        coord = cartesian_to_isometric(coord);
                        tiles_infos.x = coord.x;
                        tiles_infos.y = coord.y - TILES_HEIGHT; // offset
                        SDL_RenderCopy(renderer, tiles[id], NULL, &tiles_infos);
                    }

                    for (c=0;c<MAX_CHARACTERS;c++)
                    {
                        coord = all_characters[c].movement.position;
                        pixels_to_unit(&coord);
                        coord2.x = i; coord2.y = j;
                        if (is_same_coord(coord, coord2))
                        {
                            isometrified = cartesian_to_isometric(all_characters[c].movement.position);
                            sprites_infos.x = isometrified.x;
                            sprites_infos.y = isometrified.y;
                            state = MOVE;
                            direction = all_characters[c].movement.direction;
                            current_frame = all_characters[c].on_screen.current_frame;
                            SDL_RenderCopy(
                                    renderer,
                                    all_characters[c].on_screen.sprite,
                                    &(all_characters[c].on_screen.frames[direction][state][current_frame]),
                                    &sprites_infos
                                    );
                        }

                    }
                }

            if (paused)
            {
                tiles_infos.x = 0; tiles_infos.y = 0;
                SDL_RenderCopy(renderer, red_rect, NULL, &tiles_infos);
            }

            SDL_RenderPresent(renderer);
        }
        else // do not overuse CPU
            SDL_Delay(FRAMERATE - (time - prev_time));
    }

    free_map(&test_map);
    SDL_Quit();
    return 0;
}
