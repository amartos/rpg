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
    int x, y, xscroll = 0, yscroll = 0;
    int minx = 0, miny = 0, maxx = 0, maxy = 0;

    // SDL vars init
    SDL_Window *window; SDL_Renderer *renderer;
    init_screen(&window, &renderer);

    Image images[0xFFFF]; init_images_array(renderer, images);
    SDL_Texture *grey_rect = make_colored_rect(renderer, TILES_WIDTH, TILES_HEIGHT, 0, 0, 0, 50);

    SDL_Rect sprites_infos; init_sdl_rect(&sprites_infos);
    sprites_infos.w = SPRITES_WIDTH; sprites_infos.h = SPRITES_HEIGHT;

    SDL_Rect mouse_hover_rect; init_sdl_rect(&mouse_hover_rect);
    mouse_hover_rect.w = TILES_WIDTH; mouse_hover_rect.h = TILES_HEIGHT;

    SDL_Rect tiles_infos; init_sdl_rect(&tiles_infos);

    SDL_Event event;

    // custom structs init
    Bool done = FALSE;
    Bool paused = FALSE;
    Bool mouse_down = FALSE;

    MovementType movement_type = WALK;
    Cardinals direction = S;

    Character all_characters[MAX_CHARACTERS];
    State state = MOVE;

    Cursors mouse_type = HOVER;

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
    xscroll = (SCREEN_WIDTH/2) / TILES_WIDTH;
    yscroll = (SCREEN_HEIGHT/2) / TILES_HEIGHT;
    for (i=0;i<MAX_CHARACTERS;i++)
        init_character(&all_characters[i], i, center, SQUARE);

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
                            test_map.collisions,
                            test_map.cost
                            );
                    check_character_frame(&(all_characters[i].on_screen), time);
                }
            }

            // Rendering
            SDL_RenderClear(renderer); 

            SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF); // RGBA
            SDL_RenderDrawRect(renderer, NULL);

            apply_tiles(&renderer, test_map, all_characters, images, xscroll, yscroll);

            if (paused)
                SDL_RenderCopy(renderer, grey_rect, NULL, NULL);

            SDL_RenderCopy(renderer, images[mouse_type].texture, NULL, &mouse_hover_rect);

            for (i=0;i<MAX_CHARACTERS;i++)
            {
                if (all_characters[i].movement.moving)
                {
                    center = all_characters[i].movement.path[0];
                    center.x -= xscroll; center.y -= yscroll;
                    mouse_hover_rect = coord_to_isosdlrect(center);
                    SDL_RenderCopy(renderer, images[VALID].texture, NULL, &mouse_hover_rect);
                }
            }
        }
        else // do not overuse CPU
            SDL_Delay(FRAMERATE - (time - prev_time));

        while(SDL_PollEvent(&event))
            switch(event.type)
            {
                case SDL_QUIT:
                    done = TRUE;
                    break;
                case SDL_MOUSEMOTION:
                    center.x = event.motion.x - TILES_WIDTH/2;
                    center.y = event.motion.y - TILES_HEIGHT/2;
                    center = isometric_to_cartesian(center);
                    mouse_hover_rect = coord_to_isosdlrect(center);
                    if (!is_out_of_map(center, max_coord))
                        mouse_type = HOVER;
                    else
                        mouse_type = INVALID;
                    break;
                case SDL_MOUSEBUTTONUP:
                    if (mouse_down)
                        mouse_down = FALSE;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    if (!mouse_down)
                    {
                        mouse_down = TRUE;
                        center.x = event.button.x - TILES_WIDTH/2;
                        center.y = event.button.y - TILES_HEIGHT/2;
                        center = isometric_to_cartesian(center);
                        for (i=0;i<MAX_CHARACTERS;i++)
                        {
                            coord = center;
                            coord.x += xscroll; coord.y += yscroll;
                            deploy(
                                    &coord,
                                    determine_direction(all_characters[i].movement.position, coord),
                                    all_characters[i].movement.formation, i
                                    );
                            if (
                                    !is_same_coord(coord, all_characters[i].movement.position) &&
                                    !is_out_of_map(coord, max_coord) &&
                                    !is_colliding(coord, test_map.collisions)
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
                                stop_movement(&all_characters[i].movement);
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
                        case SDLK_F5:
                            xscroll = (SCREEN_WIDTH/2)/TILES_WIDTH;
                            yscroll = (SCREEN_HEIGHT/2)/TILES_HEIGHT;
                            break;
                        case SDLK_UP:
                            yscroll -= 1;
                            break;
                        case SDLK_DOWN:
                            yscroll += 1;
                            break;
                        case SDLK_LEFT:
                            xscroll -= 1;
                            break;
                        case SDLK_RIGHT:
                            xscroll += 1;
                            break;
                    }
                    break;
            }


        SDL_RenderPresent(renderer);
    }

    free_map(&test_map);
    free_images_array(images);
    SDL_DestroyTexture(grey_rect);

    SDL_Quit();
    return 0;
}
