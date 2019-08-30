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

enum {GREEN, RED};

int main(int argc, char *argv[])
{
    int i;

    //load screen
    SDL_Surface *screen = NULL;
    init_screen(&screen);
    int screen_bg_color[3] = {0xFF, 0xFF, 0xFF};
    set_BG_color(&screen, screen_bg_color);
    /* Oddly, for now this is not needed
     * SDL_EnableKeyRepeat(0, 0); */

    // load characters
    Character all_characters[MAX_CHARACTERS]; // 0 test_character_green, 1 test_character_red;
    SDL_Color palette[MAX_CHARACTERS][COLOR_PALETTE], green[COLOR_PALETTE], red[COLOR_PALETTE];
    palette[GREEN][0].r = 0x10, palette[GREEN][0].g = 0xA8, palette[GREEN][0].b = 0x40;
    palette[GREEN][1].r = 0xF8, palette[GREEN][1].g = 0xB8, palette[GREEN][1].b = 0x88;
    palette[GREEN][2].r = 0x18, palette[GREEN][2].g = 0x80, palette[GREEN][2].b = 0xF8;

    palette[RED][0].r = 0xF8, palette[RED][0].g = 0x00, palette[RED][0].b = 0x00;
    palette[RED][1].r = 0xF8, palette[RED][1].g = 0xB8, palette[RED][1].b = 0x88;
    palette[RED][2].r = 0x18, palette[RED][2].g = 0x80, palette[RED][2].b = 0xF8;

    for (i=0;i<MAX_CHARACTERS;i++)
    {
        init_character(&all_characters[i], palette[i], "assets/sprites/characters/test_character_grey.png", 2, TRUE, 12, 4);
        /* double size of sprites as the images are really small
         * 16 pixels w/h is too small for recent screens but
         * good for GBC, and the test sprites are from this console
         * this will be delete when real sprites are done */
        all_characters[i].sprite = rotozoomSurface(all_characters[i].sprite, 0.0, 2.0, 0.0);
        place_character(&all_characters[i], screen->w/2 + (i * 64), screen->h/2 + (i * 64));
    }

    // load maps
    Map test_map;
    init_map(&test_map, "assets/maps/test_map");
    SDL_Surface *test_tile = IMG_Load("assets/tiles/test_tile.png");
    test_tile = rotozoomSurface(test_tile, 0.0, 2.0, 0.0);

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
                            for (i=0;i<MAX_CHARACTERS;i++)
                                move_character(&all_characters[i], UP, time, test_map);
                            break;
                        case SDLK_DOWN:
                            for (i=0;i<MAX_CHARACTERS;i++)
                                move_character(&all_characters[i], DOWN, time, test_map);
                            break;
                        case SDLK_LEFT:
                            for (i=0;i<MAX_CHARACTERS;i++)
                                move_character(&all_characters[i], LEFT, time, test_map);
                            break;
                        case SDLK_RIGHT:
                            for (i=0;i<MAX_CHARACTERS;i++)
                                move_character(&all_characters[i], RIGHT, time, test_map);
                            break;
                    }
                    break;
            }
            set_BG_color(&screen, screen_bg_color);
            apply_tiles(&screen, BACKGROUND, test_map, test_tile);
            for (i=0;i<MAX_CHARACTERS;i++)
                SDL_BlitSurface(
                        all_characters[i].sprite,
                        &all_characters[i].frames[all_characters[i].direction][MOVE][all_characters[i].current_frame], 
                        screen, &all_characters[i].infos
                        );
            apply_tiles(&screen, FOREGROUND, test_map, test_tile);

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
    
    for (i=0;i<MAX_CHARACTERS;i++)
        free_character(&all_characters[i]);
    free_map(&test_map);
    SDL_FreeSurface(test_tile);
    return 0;
}