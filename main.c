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
    Character test_character_green, test_character_red;
    SDL_Color green[COLOR_PALETTE], red[COLOR_PALETTE];
    green[0].r = 0x10, green[0].g = 0xA8, green[0].b = 0x40;
    green[1].r = 0xF8, green[1].g = 0xB8, green[1].b = 0x88;
    green[2].r = 0x18, green[2].g = 0x80, green[2].b = 0xF8;

    red[0].r = 0xF8, red[0].g = 0x00, red[0].b = 0x00;
    red[1].r = 0xF8, red[1].g = 0xB8, red[1].b = 0x88;
    red[2].r = 0x18, red[2].g = 0x80, red[2].b = 0xF8;

    init_character(&test_character_green, green, "assets/sprites/characters/test_character_grey.png", 2, TRUE, 12, 4);
    init_character(&test_character_red, red, "assets/sprites/characters/test_character_grey.png", 2, TRUE, 12, 4);

    // load maps
    Map test_map;
    init_map(&test_map, "assets/maps/test_map");
    SDL_Surface *test_tile = IMG_Load("assets/tiles/test_tile.png");

    /* double size of sprites as the images are really small
     * 16 pixels w/h is too small for recent screens but
     * good for GBC, and the test sprites are from this console
     * this will be delete when real sprites are done */
    test_character_green.sprite = rotozoomSurface(test_character_green.sprite, 0.0, 2.0, 0.0);
    test_character_red.sprite = rotozoomSurface(test_character_red.sprite, 0.0, 2.0, 0.0);
    test_tile = rotozoomSurface(test_tile, 0.0, 2.0, 0.0);

    // start at center of screen
    place_character(&test_character_green, screen->w/2, screen->h/2);
    place_character(&test_character_red, test_character_green.infos.x+64, test_character_green.infos.y+64);

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
            apply_tiles(&screen, test_map.infos.x, test_map.infos.h, test_map.background_tiles, test_tile);
            SDL_BlitSurface(
                    test_character_green.sprite,
                    &test_character_green.frames[test_character_green.direction][MOVE][test_character_green.current_frame], 
                    screen, &test_character_green.infos
                    );
            SDL_BlitSurface(
                    test_character_red.sprite,
                    &test_character_red.frames[test_character_red.direction][MOVE][test_character_red.current_frame], 
                    screen, &test_character_red.infos
                    );
            apply_tiles(&screen, test_map.infos.x, test_map.infos.h, test_map.foreground_tiles, test_tile);

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

    free_character(&test_character_green);
    free_character(&test_character_red);
    free_map(&test_map);
    SDL_FreeSurface(test_tile);
    return 0;
}
