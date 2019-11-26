#include "animation.h"


void check_animation_frame(Animation *animation, unsigned int const time)
{
    unsigned int current = animation->current_frame;
    unsigned int max = animation->total_frames - 1; // arrays start at 0

    if (time - animation->time > animation->framerate)
    {
        // Go to next frame, but if last frame reached, loop back
        animation->current_frame = current >= max ? 0 : current + 1;

        // Keep track of time since frame was changed
        animation->time = time;
    }
}

void init_animation(Animation *animation, unsigned int const frames)
{
    animation->total_frames = frames;
    animation->current_frame = 0;
    animation->animated = animation->total_frames > 1 ? TRUE : FALSE;
    animation->framerate = animation->animated ? 1000/CHARACTER_FPS : 0;
    animation->state = MOVE;
    animation->time = 0;

    /* If the object is animated, store infos about how to show frames in a SDL
     * object. */
    if (animation->animated)
    {
        /* This depends on the sprite order
         * TODO: if(sprite changes), reorder
         * WARNING: This HEAVILY depends on the Direction struct order. */
        unsigned int d, s, f;
        for (d=N;d<=NW;d++) // directions
            for (s=MOVE;s<=MOVE_SHIELD;s++) // States
                for (f=0;f<animation->total_frames;f++)
                    animation->frames[d][s][f] = init_frame_sdlrect(d * 2 + f, s);
    }
}
