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
        Camera *pcamera
        );

Cursors handle_mouse_motion(
        SDL_Event const event,
        Camera const camera,
        SDL_Rect *mouse_hover_rect,
        Coord const max_coord
        );

void handle_mouse_click(
        SDL_Event event,
        Camera const camera,
        Asset characters[MAX_CHARACTERS],
        unsigned int max_objects,
        Map const map
        );

#endif // define RPG_EVENT_HANDLER
