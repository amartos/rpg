#include "errors.h"
#include "try_throw_catch.h"
#include "macros.h"

#include "screen.h"
#include "characters.h"
#include "movement.h"
#include "map.h"

#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
// this should be removed at some point
#include <SDL/SDL_rotozoom.h>

enum {GREEN, RED};

int main(int argc, char *argv[])
{
    int i;
    Coord start_position; init_coord(&start_position);

    //load screen
    SDL_Surface *screen = NULL;
    init_screen(&screen);
    int screen_bg_color[3] = {0xFF, 0xFF, 0xFF};
    set_BG_color(&screen, screen_bg_color);
    /* Oddly, for now this is not needed
     * SDL_EnableKeyRepeat(0, 0); */

    // load characters
    Character all_characters[MAX_CHARACTERS]; // 0 test_character_green, 1 test_character_red;
    SDL_Color palette[MAX_CHARACTERS][COLOR_PALETTE];
    palette[GREEN][0].r = 0x10, palette[GREEN][0].g = 0xA8, palette[GREEN][0].b = 0x40;
    palette[GREEN][1].r = 0xF8, palette[GREEN][1].g = 0xB8, palette[GREEN][1].b = 0x88;
    palette[GREEN][2].r = 0x18, palette[GREEN][2].g = 0x80, palette[GREEN][2].b = 0xF8;

    palette[RED][0].r = 0xF8, palette[RED][0].g = 0x00, palette[RED][0].b = 0x00;
    palette[RED][1].r = 0xF8, palette[RED][1].g = 0xB8, palette[RED][1].b = 0x88;
    palette[RED][2].r = 0x18, palette[RED][2].g = 0x80, palette[RED][2].b = 0xF8;

    for (i=0;i<MAX_CHARACTERS;i++)
    {
        start_position.x = screen->w/2 + (i*64);
        start_position.y = screen->h/2 + (i*64);
        init_character(
                &all_characters[i],
                palette[i],
                "assets/sprites/characters/test_character_grey.png",
                2, TRUE, 12, 4, // N frames, moving, FPS, velocity
                start_position
                );
        /* double size of sprites as the images are really small
         * 16 pixels w/h is too small for recent screens but
         * good for GBC, and the test sprites are from this console
         * this will be delete when real sprites are done */
        all_characters[i].sprite = rotozoomSurface(all_characters[i].sprite, 0.0, 2.0, 0.0);
    }

    // load maps
    Map test_map; init_map(&test_map, "assets/maps/test_map2");
    SDL_Surface *test_tile = IMG_Load("assets/tiles/test_tile.png");
    test_tile = rotozoomSurface(test_tile, 0.0, 2.0, 0.0);
    Coord max_coord; init_coord(&max_coord);
    max_coord.x = test_map.w;//x_tiles;
    max_coord.y = test_map.h;//y_tiles;

    // main loop
    Bool done = FALSE;
    Bool movement = FALSE;
    MovementType movement_type = WALK;
    unsigned int nodes = 0;
    unsigned int current_node = 0;
    int time = 0, prev_time = 0;
    Coord goal; init_coord(&goal);
    SDL_Event event;
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
                    switch(event.button.button)
                    {
                        case SDL_BUTTON_LEFT:
                            movement = TRUE;
                            movement_type = PATH;
                            for (i=MAX_CHARACTERS-1;i>=0;i--)
                            {
                                goal.x = (i * 64) + event.button.x;
                                goal.y = (i * 64) + event.button.y;
                                nodes = find_path(
                                        &(all_characters[i].path),
                                        all_characters[i].position,
                                        goal,
                                        max_coord,
                                        test_map.schematics[COLLISIONS],
                                        test_map.schematics[COST]
                                        );

                                if (nodes && all_characters[i].path != NULL)
                                {
                                    all_characters[i].nodes = nodes;
                                    nodes = 0;
                                }
                            }
                            break;
                        case SDL_BUTTON_RIGHT:
                            movement = TRUE;
                            movement_type = TELEPORT;
                            for (i=0;i<MAX_CHARACTERS;i++)
                            {
                                movement_type = TELEPORT;
                                all_characters[i].goal.x = (i * 64) + event.button.x;
                                all_characters[i].goal.y = (i * 64) + event.button.y;
                            }
                            break;
                    }
                    break;
                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym)
                    {
                        case SDLK_UP:
                            movement = TRUE;
                            for (i=0;i<MAX_CHARACTERS;i++)
                            {
                                all_characters[i].direction = UP;
                                all_characters[i].goal.x = all_characters[i].position.x;
                                all_characters[i].goal.y = all_characters[i].position.y;
                                all_characters[i].goal.y -= all_characters[i].velocity;
                            }
                            break;
                        case SDLK_DOWN:
                            movement = TRUE;
                            for (i=0;i<MAX_CHARACTERS;i++)
                            {
                                all_characters[i].direction = DOWN;
                                all_characters[i].goal.x = all_characters[i].position.x;
                                all_characters[i].goal.y = all_characters[i].position.y;
                                all_characters[i].goal.y += all_characters[i].velocity;
                            }
                            break;
                        case SDLK_LEFT:
                            movement = TRUE;
                            for (i=0;i<MAX_CHARACTERS;i++)
                            {
                                all_characters[i].direction = LEFT;
                                all_characters[i].goal.x = all_characters[i].position.x;
                                all_characters[i].goal.y = all_characters[i].position.y;
                                all_characters[i].goal.x -= all_characters[i].velocity;
                            }
                            break;
                        case SDLK_RIGHT:
                            movement = TRUE;
                            for (i=0;i<MAX_CHARACTERS;i++)
                            {
                                all_characters[i].direction = RIGHT;
                                all_characters[i].goal.x = all_characters[i].position.x;
                                all_characters[i].goal.y = all_characters[i].position.y;
                                all_characters[i].goal.x += all_characters[i].velocity;
                            }
                            break;
                    }
                    break;
            }

            if (movement)
            {
                for (i=0;i<MAX_CHARACTERS;i++)
                {
                    if (all_characters[i].path != NULL && (movement_type == PATH || all_characters[i].nodes))
                    {
                        current_node = all_characters[i].nodes;
                        if (is_same_coord(all_characters[i].path[current_node], all_characters[i].position))
                        {
                            all_characters[i].nodes--;
                            current_node = all_characters[i].nodes;
                        }
                        all_characters[i].goal = all_characters[i].path[current_node];
                    }

                    if (!all_characters[i].nodes && all_characters[i].path != NULL)
                    {
                        all_characters[i].path = NULL;
                        all_characters[i].goal.x = 0;
                        all_characters[i].goal.y = 0;
                    }

                    all_characters[i].direction = move(
                            &(all_characters[i].position),
                            &(all_characters[i].goal),
                            movement_type,
                            max_coord,
                            test_map.schematics[COLLISIONS],
                            all_characters[i].velocity
                            );

                    if (!(all_characters[i].goal.x || all_characters[i].goal.y) && all_characters[i].path == NULL)
                        movement = FALSE;
                    if (time - all_characters[i].previous_time > all_characters[i].framerate)
                    {
                        all_characters[i].current_frame++;
                        if (all_characters[i].current_frame >= all_characters[i].number_of_frames)
                            all_characters[i].current_frame = 0;
                        all_characters[i].previous_time = time;
                    }
                }
                if (movement_type != WALK)
                    movement_type = WALK;
            }

            set_BG_color(&screen, screen_bg_color);
            apply_tiles(&screen, BACKGROUND, test_map, test_tile);
            for (i=0;i<MAX_CHARACTERS;i++)
            {
                // TODO: two sets of same coord is useless
                all_characters[i].infos.x = all_characters[i].position.x;
                all_characters[i].infos.y = all_characters[i].position.y;
                SDL_BlitSurface(
                        all_characters[i].sprite,
                        &all_characters[i].frames[all_characters[i].direction][MOVE][all_characters[i].current_frame], 
                        screen, &all_characters[i].infos
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
