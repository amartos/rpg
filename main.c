#include "main.h"


static void handle_movement(Movement *movement, Map const map)
{
    unsigned int nodes = 0;
    Coord max_coord; init_coord(&max_coord);
    max_coord.x = map.x_tiles * TILES_WIDTH;
    max_coord.y = map.y_tiles * TILES_HEIGHT;

    switch (movement->movement_type)
    {
        case PATH:
            movement->movement_type = WALK;
            nodes = find_path(movement, map);
            if (!nodes)
                goto end_move;
            goto make_a_move;
        default:
            goto make_a_move;
        make_a_move:
            move(movement, max_coord, map.schematics[COLLISIONS]);
            if (is_same_coord(movement->path[0], movement->position))
                goto end_move;
            else
                break;
        end_move:
            movement->path[0].x = 0;
            movement->path[0].y = 0;
            movement->moving = FALSE;
            break;
    }
}

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

static void fire_movement(Character characters[MAX_CHARACTERS], MovementType const movement)
{
    unsigned int i;
    for (i=0;i<MAX_CHARACTERS;i++)
    {
        characters[i].movement.moving = TRUE;
        characters[i].movement.movement_type = movement;
    }
}

static void change_formation(Character characters[MAX_CHARACTERS], Deployment formation)
{
    unsigned int i;
    for (i=0;i<MAX_CHARACTERS;i++)
        get_formation_offset(&(characters[i].movement.position), i, formation);
    fire_movement(characters, PATH);
}

int main(int argc, char *argv[])
{
    unsigned int i = 0, current_frame = 0;
    unsigned int screen_bg_color[3] = {0xFF, 0xFF, 0xFF};
    unsigned int time = 0, prev_time = 0;

    // SDL vars init
    SDL_Surface *screen = NULL; init_screen(&screen);
    SDL_Surface *test_tile = IMG_Load("assets/tiles/test_tile.png");
    SDL_Rect infos;
    infos.x = 0; infos.y = 0;
    infos.w = SPRITES_WIDTH; infos.h = SPRITES_HEIGHT;
    SDL_Event event;

    // custom structs init
    Bool done = FALSE;

    MovementType movement_type = WALK;
    Cardinals direction = S;

    Character all_characters[MAX_CHARACTERS];
    State state = MOVE;

    Coord max_coord; init_coord(&max_coord);
    Coord center; init_coord(&center);
    Coord position; init_coord(&position);
    Coord start_position[MAX_CHARACTERS];
    for (i=0;i<MAX_CHARACTERS;i++)
        init_coord(start_position+i);

    Map test_map; init_map(&test_map, "assets/maps/test_map2");

    // load characters
    center.x = TILES_WIDTH * 4;
    center.y = TILES_HEIGHT * 4;
    for (i=0;i<MAX_CHARACTERS;i++)
    {
        start_position[i] = center;
        get_formation_offset(&start_position[i], i, SQUARE);
        init_character(&all_characters[i], i, start_position[i]);
    }

    test_tile = rotozoomSurface(test_tile, 0.0, 2.0, 0.0);
    max_coord.x = test_map.w;
    max_coord.y = test_map.h;

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
                    for (i=0;i<MAX_CHARACTERS;i++)
                    {
                        // add offset to center
                        center.ox = all_characters[i].movement.position.ox;
                        center.oy = all_characters[i].movement.position.oy;
                        all_characters[i].movement.path[0] = center;
                    }
                    switch(event.button.button)
                    {
                        case SDL_BUTTON_LEFT:
                            fire_movement(all_characters, PATH);
                            break;
                        case SDL_BUTTON_RIGHT:
                            fire_movement(all_characters, TELEPORT);
                            break;
                    }
                    break;
                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym)
                    {
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

            set_BG_color(&screen, screen_bg_color);
            apply_tiles(&screen, BACKGROUND, test_map, test_tile);
            for (i=0;i<MAX_CHARACTERS;i++)
            {
                if (all_characters[i].movement.moving)
                {
                    handle_movement(&all_characters[i].movement, test_map);
                    check_character_frame(&(all_characters[i].on_screen), time);
                }
                position = offsetting(all_characters[i].movement.position);
                infos.x = position.x;
                infos.y = position.y;
                state = MOVE;
                direction = all_characters[i].movement.direction;
                current_frame = all_characters[i].on_screen.current_frame;
                SDL_BlitSurface(
                        all_characters[i].on_screen.sprite,
                        &(all_characters[i].on_screen.frames[direction][state][current_frame]),
                        screen, &infos
                        );
            }
            apply_tiles(&screen, FOREGROUND, test_map, test_tile);

            TRY
            {
                if (SDL_Flip(screen))
                    THROW(FLIP_SCREEN_FAILURE);
            }
            CATCH(FLIP_SCREEN_FAILURE)
            {
                logger(FLIP_SCREEN_FAILURE, SDL_GetError());
                exit(EXIT_FAILURE);
            }
            ETRY;
        }
        else // do not overuse CPU
            SDL_Delay(FRAMERATE - (time - prev_time));
    }

    for (i=0;i<MAX_CHARACTERS;i++)
        free_character(&all_characters[i]);
    free_map(&test_map);
    SDL_FreeSurface(test_tile);
    return 0;
}
