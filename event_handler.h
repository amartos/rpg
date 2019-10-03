#ifndef RPG_EVENT_HANDLER
#define RPG_EVENT_HANDLER

#include "macros.h"

#include "coord.h"
#include "images.h"
#include "animation.h"
#include "assets.h"
#include "map.h"

#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>


void handle_keyboard(
        SDL_Event event, Bool *paused,
        Asset characters[MAX_CHARACTERS],
        Coord *scroll
        );

Cursors handle_mouse_motion(
        SDL_Event const event,
        Coord const scroll,
        SDL_Rect *mouse_hover_rect,
        Coord const max_coord
        );

void handle_mouse_click(
        SDL_Event event,
        Coord const scroll,
        Asset characters[MAX_CHARACTERS],
        unsigned int max_objects,
        Map const map
        );

#endif // define RPG_EVENT_HANDLER
