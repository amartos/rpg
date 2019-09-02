#include "main.h"


static void handle_movement(
        Character *character,
        Coord const max_coord,
        Map const map
        )
{
    unsigned int current_node = 0, nodes = 0;

    switch (character->movement_type)
    {
        case PATH:
            character->movement_type = WALK;
            if (character->path != NULL)
            {
                free_path(character);
                character->nodes = 0;
            }

            nodes = find_path(
                    &character->path,
                    character->position,
                    character->goal,
                    character->velocity,
                    max_coord,
                    map.schematics[COLLISIONS],
                    map.schematics[COST]
                    );

            if (nodes && character->path != NULL)
            {
                character->nodes = nodes;
                current_node = character->nodes;
                character->goal = character->path[current_node];
                goto make_a_move;
            }
            else
                goto end_move;
        case TELEPORT:
            if (character->path != NULL)
            {
                free_path(character);
                character->nodes = 0;
            }
            goto make_a_move;
        case WALK:
            current_node = character->nodes;
            if (is_same_coord(character->path[current_node], character->position))
            {
                if (current_node)
                {
                    character->nodes--;
                    current_node = character->nodes;
                    character->goal = character->path[current_node];
                }
                else
                {
                    free_path(character);
                    goto end_move;
                }
            }
            else
                character->goal = character->path[current_node];
            goto make_a_move;
        make_a_move:
            character->direction = move(
                    &character->position,
                    character->goal,
                    character->movement_type,
                    max_coord,
                    map.schematics[COLLISIONS],
                    character->velocity
                    );
            if (is_same_coord(character->goal, character->position))
                goto end_move;
            else
                break;
        end_move:
            if (character->path == NULL)
            {
                character->goal.x = 0;
                character->goal.y = 0;
                character->moving = FALSE;
            }
            break;
    }
}

static void check_character_frame(Character *character, unsigned int const time)
{
    if (time - character->previous_time > character->framerate)
    {
        character->current_frame++;
        if (character->current_frame >= character->number_of_frames)
            character->current_frame = 0;
        character->previous_time = time;
    }
}

static void fire_movement(Character characters[MAX_CHARACTERS], MovementType const movement)
{
    unsigned int i;
    for (i=0;i<MAX_CHARACTERS;i++)
    {
        characters[i].moving = TRUE;
        characters[i].movement_type = movement;
    }
}

static void change_formation(Character characters[MAX_CHARACTERS], Deployment formation)
{
    unsigned int i;
    for (i=0;i<MAX_CHARACTERS;i++)
        get_formation_offset(&(characters[i].position), i, formation);
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
    SDL_Color palette[MAX_CHARACTERS][COLOR_PALETTE];
    SDL_Rect infos;
    infos.x = 0; infos.y = 0;
    infos.w = SPRITES_WIDTH; infos.h = SPRITES_HEIGHT;
    SDL_Event event;

    // custom structs init
    Bool done = FALSE;

    MovementType movement_type = WALK;
    Direction direction = UP;

    Character all_characters[MAX_CHARACTERS];
    State state = MOVE;

    Coord max_coord; init_coord(&max_coord);
    Coord center; init_coord(&center);
    Coord position; init_coord(&position);
    Coord start_position[MAX_CHARACTERS];
    for (i=0;i<MAX_CHARACTERS;i++)
        init_coord(start_position+i);

    Map test_map; init_map(&test_map, "assets/maps/test_map2");

    // load characters palette : 0 green, 1 red, 2 blue, 3 yellow
    palette[GREEN][0].r = 0x10, palette[GREEN][0].g = 0xA8, palette[GREEN][0].b = 0x40;
    palette[RED][0].r = 0xF8, palette[RED][0].g = 0x00, palette[RED][0].b = 0x00;
    palette[BLUE][0].r = 0x18, palette[BLUE][0].g = 0x80, palette[BLUE][0].b = 0xf8;
    palette[YELLOW][0].r = 0xf0, palette[YELLOW][0].g = 0xe8, palette[YELLOW][0].b = 0x18;

    palette[GREEN][1].r = 0xF8, palette[GREEN][1].g = 0xB8, palette[GREEN][1].b = 0x88;
    palette[GREEN][2].r = 0x18, palette[GREEN][2].g = 0x80, palette[GREEN][2].b = 0xF8;

    palette[RED][1].r = 0xF8, palette[RED][1].g = 0xB8, palette[RED][1].b = 0x88;
    palette[RED][2].r = 0x18, palette[RED][2].g = 0x80, palette[RED][2].b = 0xF8;

    palette[BLUE][1].r = 0xF8, palette[BLUE][1].g = 0xB8, palette[BLUE][1].b = 0x88;
    palette[BLUE][2].r = 0x18, palette[BLUE][2].g = 0x80, palette[BLUE][2].b = 0xF8;

    palette[YELLOW][1].r = 0xF8, palette[YELLOW][1].g = 0xB8, palette[YELLOW][1].b = 0x88;
    palette[YELLOW][2].r = 0x18, palette[YELLOW][2].g = 0x80, palette[YELLOW][2].b = 0xF8;

    // load characters
    center.x = TILES_WIDTH * 4;
    center.y = TILES_HEIGHT * 4;
    for (i=0;i<MAX_CHARACTERS;i++)
    {
        start_position[i] = center;
        get_formation_offset(&start_position[i], i, SQUARE);
        init_character(
                &all_characters[i],
                palette[i],
                "assets/sprites/characters/test_character_grey.png",
                2, 12, 4, // N frames, FPS, velocity
                start_position[i]
                );
        // double size of sprites as the images are really small
        // 16 pixels w/h is too small for recent screens but
        // good for GBC, and the test sprites are from this console
        // this will be delete when real sprites are done
        all_characters[i].sprite = rotozoomSurface(all_characters[i].sprite, 0.0, 2.0, 0.0);
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
                if (all_characters[i].moving)
                {
                    // add offset to center
                    center.ox = all_characters[i].position.ox;
                    center.oy = all_characters[i].position.oy;
                    all_characters[i].goal = center;
                    handle_movement(&all_characters[i], max_coord, test_map);
                    check_character_frame(&all_characters[i], time);
                }
                position = offsetting(all_characters[i].position);
                infos.x = position.x;
                infos.y = position.y;
                state = MOVE;
                direction = all_characters[i].direction;
                current_frame = all_characters[i].current_frame;
                SDL_BlitSurface(
                        all_characters[i].sprite,
                        &all_characters[i].frames[direction][state][current_frame],
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
