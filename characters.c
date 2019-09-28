#include "characters.h"


void change_formation(Character characters[MAX_CHARACTERS], Deployment formation)
{
    unsigned int i;
    for (i=0;i<MAX_CHARACTERS;i++)
    {
        characters[i].movement.formation = formation;
        fire_movement(&characters[i].movement, PATH);
    }
}

void check_character_frame(OnScreen *on_screen, unsigned int const time)
{
    if (time - on_screen->time > on_screen->framerate)
    {
        on_screen->current_frame++;
        if (on_screen->current_frame >= on_screen->total_frames)
            on_screen->current_frame = 0;
        on_screen->time = time;
    }
}

void make_char_move(
        Character characters[MAX_CHARACTERS],
        Map const map,
        unsigned int const time,
        Bool paused
        )
{
    unsigned int i;
    Coord max_coord; init_coord(&max_coord);
    max_coord.x = map.maxx; max_coord.y = map.maxy;
    for (i=0;i<MAX_CHARACTERS;i++)
    {
        if (characters[i].movement.moving && !paused)
        {
            move(
                    &characters[i].movement,
                    max_coord,
                    map.collisions,
                    map.cost
                    );
            check_character_frame(&(characters[i].on_screen), time);
        }
    }
}

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

void init_character(Character *character)
{
    init_movement(&character->movement);
    init_on_screen(&character->on_screen);
}
