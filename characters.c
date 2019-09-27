#include "characters.h"


static void init_on_screen(OnScreen *on_screen)
{
    unsigned int i, j, f;

    //-------------------------------------------------------------------------
    // Frames
    //-------------------------------------------------------------------------

    on_screen->total_frames = 2;

    // This depends on the sprite order
    // TODO: if(sprite changes), reorder
    for (i=N;i<=NW;i++) // directions
        for (j=MOVE;j<=MOVE_SHIELD;j++) // States
            for (f=0;f<on_screen->total_frames;f++)
            {
                on_screen->frames[i][j][f].x = (i * 2 + f) * SPRITES_WIDTH/2;
                on_screen->frames[i][j][f].y = j * SPRITES_HEIGHT/2;
                on_screen->frames[i][j][f].w = SPRITES_WIDTH/2;
                on_screen->frames[i][j][f].h = SPRITES_HEIGHT/2;
            }

    on_screen->current_frame = 0;

    //-------------------------------------------------------------------------
    // Constants
    //-------------------------------------------------------------------------

    on_screen->animated = FALSE;
    on_screen->framerate = 0;
    if (on_screen->total_frames > 1)
    {
        on_screen->animated = TRUE;
        on_screen->framerate = 1000/12; // 1000/character FPS
    }

    on_screen->state = MOVE;
    on_screen->time = 0;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void init_character(
        Character *character,
        unsigned int const id,
        Coord const start_position,
        Deployment const formation
        )
{
    character->id = id;
    init_movement(&character->movement, start_position, formation, id);
    init_on_screen(&character->on_screen);
}
