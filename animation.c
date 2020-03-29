#include "animation.h"


void check_animation_frame(Animation *animation, int const time)
{
    int current = animation->current_frame;
    int max = animation->total_frames - 1; // arrays start at 0

    if (time - animation->time > animation->framerate)
    {
        // Go to next frame, but if last frame reached, loop back
        animation->current_frame = current >= max ? 0 : current + 1;

        // Keep track of time since frame was changed
        animation->time = time;
    }
}

void init_animation(Animation *animation, int const frames)
{
    animation->total_frames = frames;
    animation->current_frame = 0;
    animation->animated = animation->total_frames > 1 ? TRUE : FALSE;
    animation->framerate = animation->animated ? (int)(1000/CHARACTER_FPS) : 0;
    animation->state = MOVE;
    animation->time = 0;

    /* If the object is animated, store infos about how to show frames in a SDL
     * object. */
    if (animation->animated)
    {
        /* int d, s, f; */
        /* for (d=N;d<=NW;d++) // directions */
        /*     for (s=MOVE;s<=MOVE_SHIELD;s++) // States */
        /*         for (f=0;f<animation->total_frames;f++) */
        /*             animation->frames[d][s][f] = init_frame_sdlrect(d * 2 + f, s); */
        for (int f=0;f<animation->total_frames;f++)
        {
            /* The new images used are not in the right order, thus this is set
             * manually.
             * TODO: autmate this by modifying the sprites. */
            animation->frames[N][MOVE][f] = init_frame_sdlrect(f, 3);
            animation->frames[NE][MOVE][f] = init_frame_sdlrect(f, 3);
            animation->frames[E][MOVE][f] = init_frame_sdlrect(f, 1);
            animation->frames[SE][MOVE][f] = init_frame_sdlrect(f, 1);
            animation->frames[S][MOVE][f] = init_frame_sdlrect(f, 0);
            animation->frames[SW][MOVE][f] = init_frame_sdlrect(f, 0);
            animation->frames[W][MOVE][f] = init_frame_sdlrect(f, 0);
            animation->frames[NW][MOVE][f] = init_frame_sdlrect(f, 2);
        }
    }
}
