#include "animation.h"


void change_animobj_formation(AnimatedObject objects[], unsigned int max, Deployment formation)
{
    unsigned int i;
    for (i=0;i<max;i++)
    {
        objects[i].movement.formation = formation;
        fire_movement(&objects[i].movement, PATH);
    }
}

void check_animobj_frame(Animation *animation, unsigned int const time)
{
    if (time - animation->time > animation->framerate)
    {
        animation->current_frame++;
        if (animation->current_frame >= animation->total_frames)
            animation->current_frame = 0;
        animation->time = time;
    }
}

void make_animobj_move(
        AnimatedObject objects[],
        unsigned int max,
        Map const map,
        unsigned int const time,
        Bool paused
        )
{
    unsigned int i;
    Coord max_coord; init_coord(&max_coord);
    max_coord.x = map.maxx; max_coord.y = map.maxy;
    for (i=0;i<max;i++)
    {
        if (objects[i].movement.moving && !paused)
        {
            move(
                    &objects[i].movement,
                    max_coord,
                    map.collisions,
                    map.cost
                    );
            check_animobj_frame(&objects[i].animation, time);
        }
    }
}

static void init_animation(Animation *animation)
{
    unsigned int i, j, f;

    //-------------------------------------------------------------------------
    // Frames
    //-------------------------------------------------------------------------

    animation->total_frames = 2;

    // This depends on the sprite order
    // TODO: if(sprite changes), reorder
    for (i=N;i<=NW;i++) // directions
        for (j=MOVE;j<=MOVE_SHIELD;j++) // States
            for (f=0;f<animation->total_frames;f++)
            {
                animation->frames[i][j][f].x = (i * 2 + f) * SPRITES_WIDTH/2;
                animation->frames[i][j][f].y = j * SPRITES_HEIGHT/2;
                animation->frames[i][j][f].w = SPRITES_WIDTH/2;
                animation->frames[i][j][f].h = SPRITES_HEIGHT/2;
            }

    animation->current_frame = 0;

    //-------------------------------------------------------------------------
    // Constants
    //-------------------------------------------------------------------------

    animation->animated = FALSE;
    animation->framerate = 0;
    if (animation->total_frames > 1)
    {
        animation->animated = TRUE;
        animation->framerate = 1000/12; // 1000/character FPS
    }

    animation->state = MOVE;
    animation->time = 0;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void init_animobj(AnimatedObject *object)
{
    object->id = 0;
    init_movement(&object->movement);
    init_animation(&object->animation);
}
