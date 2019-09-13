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
    max_coord.x = test_map.maxx;
    max_coord.y = test_map.maxy;

    // load characters
    center.x = 4.0;
    center.y = 4.0;
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
    SDL_FreeSurface(red_rect_surface);
    for (i=0;i<=INVALID;i++)
        SDL_FreeSurface(mouse_hover_surfaces[i]);

    // main loop
    while (!done)
    {
        time = SDL_GetTicks();
        if (time - prev_time > FRAMERATE)
        {
            prev_time = time;
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
        }
        else // do not overuse CPU
            SDL_Delay(FRAMERATE - (time - prev_time));

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
                    coord2.x = i; coord2.y = j;
                    if (is_same_coord(coord, coord2))
                    {
                        isometrified = cartesian_to_isometric(coord);
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


        for (i=0;i<MAX_CHARACTERS;i++)
        {
            if (all_characters[i].movement.moving)
            {
                coord = all_characters[i].movement.path[0];
                isometrified = cartesian_to_isometric(coord);
                mouse_hover_rect.x = isometrified.x;
                mouse_hover_rect.y = isometrified.y;
                SDL_RenderCopy(renderer, mouse[VALID], NULL, &mouse_hover_rect);
            }
        }

        SDL_PollEvent(&event);
        switch(event.type)
        {
            case SDL_QUIT:
                done = TRUE;
                break;

            case SDL_MOUSEMOTION:
                center.x = event.motion.x;
                center.y = event.motion.y;
                center = isometric_to_cartesian(center);
                if (!is_out_of_map(center, max_coord))
                {
                    mouse_hover_rect.x = event.motion.x;
                    mouse_hover_rect.y = event.motion.y;
                    SDL_RenderCopy(renderer, mouse[HOVER], NULL, &mouse_hover_rect);
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                center.x = event.button.x;
                center.y = event.button.y;
                center = isometric_to_cartesian(center);
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
                            !is_colliding(coord, test_map.schematics[COLLISIONS])
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
                        mouse_hover_rect.x = event.button.x;
                        mouse_hover_rect.y = event.button.y;
                        SDL_RenderCopy(renderer, mouse[INVALID], NULL, &mouse_hover_rect);

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


        SDL_RenderPresent(renderer);
    }

    free_map(&test_map);
    SDL_Quit();
    return 0;
}
