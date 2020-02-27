#ifndef RPG_EVENT_HANDLER
#define RPG_EVENT_HANDLER

#include "macros.h"

#include "navigation.h"
#include "images.h"
#include "assets.h"
#include "screen.h"

#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

#define SCALE_CURSOR \
    for (unsigned int i=HOVER;i<=INVALID;i++) \
        set_image_size( \
                &assets[i].image->sdlrect, \
                TILES_WIDTH(*camera), \
                TILES_HEIGHT(*camera) \
                );

#define CHANGE_GROUP_FORMATION(f) \
            for (unsigned int i=FIRST_CHAR_ID;i<=LAST_CHAR_ID;i++) \
                change_formation(assets[i].movement, f);

/* This is the main function used to handle all types of events either coming
 * from keyboard or mouse */
void handle_event(
        SDL_Event const event,
        Asset assets[0xFFFF],
        Camera *camera,
        Cursors *mouse,
        Map const map,
        Bool *paused,
        Bool *quit
        );

#endif // define RPG_EVENT_HANDLER
