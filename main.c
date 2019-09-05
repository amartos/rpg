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
        characters[i].movement.formation = formation;
    fire_movement(characters, PATH);
}

int main(int argc, char *argv[])
{
    unsigned int i = 0, current_frame = 0;
    unsigned int screen_bg_color[3] = {0xFF, 0xFF, 0xFF};
    unsigned int time = 0, prev_time = 0;

    // SDL vars init
    SDL_Surface *screen = NULL; init_screen(&screen);

    SDL_Surface* tiles[0xFFFF];
    for (i=0;i<0xFFFF;i++)
        tiles[i] = NULL;
    SDL_Surface *floor = IMG_Load("assets/tiles/tile.png");
    SDL_Surface *wall = IMG_Load("assets/tiles/wall.png");
    SDL_Surface *wall1 = IMG_Load("assets/tiles/wall1.png");

    SDL_Rect infos;
    infos.x = 0; infos.y = 0;
    infos.w = SPRITES_WIDTH; infos.h = SPRITES_HEIGHT;
    SDL_Event event;

    // custom structs init
    Bool done = FALSE;
    Bool paused = FALSE;

    MovementType movement_type = WALK;
    Cardinals direction = S;

    Character all_characters[MAX_CHARACTERS];
    State state = MOVE;

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
        init_character(&all_characters[i], i, center, SQUARE);

    // Make it double the TILE_WIDTH/HEIGHT to stick tiles between them
    floor = rotozoomSurface(floor, 0.0, 0.5, 0.0);
    wall = rotozoomSurface(wall, 0.0, 0.5, 0.0);
    wall1 = rotozoomSurface(wall1, 0.0, 0.5, 0.0);
    tiles[0x0100] = floor;
    tiles[0x0101] = wall;
    tiles[0x0102] = wall1;


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
                    for (i=0;i<MAX_CHARACTERS;i++)
                    {
                        all_characters[i].movement.current_node = 0;
                        all_characters[i].movement.path[0] = center;
                        formation_offsetting(
                                &all_characters[i].movement.path[0],
                                i,
                                all_characters[i].movement.formation
                                );
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

            set_BG_color(&screen, NULL, screen_bg_color);
            apply_tiles(&screen, BACKGROUND, test_map, tiles);
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
                isometrified = cartesian_to_isometric(all_characters[i].movement.position);
                infos.x = isometrified.x;
                infos.y = isometrified.y;
                state = MOVE;
                direction = all_characters[i].movement.direction;
                current_frame = all_characters[i].on_screen.current_frame;
                SDL_BlitSurface(
                        all_characters[i].on_screen.sprite,
                        &(all_characters[i].on_screen.frames[direction][state][current_frame]),
                        screen, &infos
                        );
            }
            apply_tiles(&screen, FOREGROUND, test_map, tiles);
            if (paused)
            {
                infos.x = 0; infos.y = 0;
                SDL_FillRect(screen, &infos, SDL_MapRGB(screen->format, 255, 0, 0));
            }

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
    for (i=0;i<0xFFFF;i++)
        if (tiles[i] != NULL)
            SDL_FreeSurface(tiles[i]);
    SDL_Quit();
    return 0;
}
