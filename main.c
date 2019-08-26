#include "errors.h"
#include "try_throw_catch.h"
#include "macros.h"

#include "screen.h"
#include "characters.h"
#include "map.h"

#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
// this should be removed at some point
#include <SDL/SDL_rotozoom.h>


int main(int argc, char *argv[])
{
    //load screen
    SDL_Surface *screen = NULL;
    init_screen(&screen);
    int screen_bg_color[3] = {0xFF, 0xFF, 0xFF};
    set_BG_color(&screen, screen_bg_color);
    /* Oddly, for now this is not needed
     * SDL_EnableKeyRepeat(0, 0); */

    // load characters
    Character test_character;
    init_character(&test_character, "assets/sprites/characters/test_character.png", 2, TRUE, 12, 4);

    // load maps
    Map test_map;
    init_map(&test_map, "assets/maps/test_map");
    SDL_Surface *test_tile = IMG_Load("assets/tiles/test_tile.png");

    /* double size of sprites as the images are really small
     * 16 pixels w/h is too small for recent screens but
     * good for GBC, and the test sprites are from this console
     * this will be delete when real sprites are done */
    test_character.sprite = rotozoomSurface(test_character.sprite, 0.0, 2.0, 0.0);
    test_tile = rotozoomSurface(test_tile, 0.0, 2.0, 0.0);

    // start at center of screen
    place_character(&test_character, screen->w/2, screen->h/2);

    // main loop
    int done = FALSE;
    int time = 0, prev_time = 0;
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
                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym)
                    {
                        case SDLK_UP:
                            move_character(&test_character, UP, time, test_map);
                            break;
                        case SDLK_DOWN:
                            move_character(&test_character, DOWN, time, test_map);
                            break;
                        case SDLK_LEFT:
                            move_character(&test_character, LEFT, time, test_map);
                            break;
                        case SDLK_RIGHT:
                            move_character(&test_character, RIGHT, time, test_map);
                            break;
                    }
                    break;
            }
            set_BG_color(&screen, screen_bg_color);
            apply_background(&screen, test_map, test_tile);
            SDL_BlitSurface(
                    test_character.sprite,
                    &test_character.frames[test_character.direction][MOVE][test_character.current_frame], 
                    screen, &test_character.infos
                    );

            TRY
            {
                if (SDL_Flip(screen) == -1)
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

    free_character(&test_character);
    free_map(&test_map);
    SDL_FreeSurface(test_tile);
    return 0;
}
