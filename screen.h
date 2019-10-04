#ifndef RPG_SCREEN
#define RPG_SCREEN

#include "errors.h"
#include "try_throw_catch.h"
#include "macros.h"

#include "coord.h"
#include "map.h"
#include "images.h"
#include "animation.h"
#include "assets.h"

#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define CALC_MIN_MAX_SCROLL \
    int minx = camera.scroll.x - 1; \
    int maxx = camera.scroll.x + SCREEN_WIDTH/TILES_WIDTH; \
    int miny = camera.scroll.y - 1; \
    int maxy = camera.scroll.y + SCREEN_HEIGHT/TILES_HEIGHT;


// prototypes
void init_screen(SDL_Window **window, SDL_Renderer **renderer);
void init_sdl_rect(SDL_Rect *rect);

// SDL_Surface tile will change for an array once more tiles are included
void apply_tiles(
        SDL_Renderer *renderer,
        Asset assets[0xFFFF],
        Map const map,
        Camera camera,
        Bool grid,
        unsigned int const level
        );

void render_screen(
        SDL_Renderer *renderer,
        Asset assets[0xFFFF],
        SDL_Texture *pause_layer,
        Cursors mouse_type,
        SDL_Rect mouse_hover_rect,
        Camera camera,
        Map const map,
        Bool paused
        );
#endif // define RPG_SCREEN
