#include "animation.h"


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

void init_animation(Animation *animation, unsigned int const frames)
{
    unsigned int i, j, f;

    //-------------------------------------------------------------------------
    // Frames
    //-------------------------------------------------------------------------

    animation->total_frames = frames;

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
