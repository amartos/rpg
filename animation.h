#ifndef RPG_CHARACTERS
#define RPG_CHARACTERS

#include "macros.h"

#include "navigation.h"
#include "images.h"

#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

/* The general FPS influences the character FPS too.
 * For now the FPS is too high to distinguish the characters movements properly,
 * but this is done on purpose during development. To observe the characters
 * movement, reduce the FPS == CHARACTER_FPS */
#define FPS 120
#define CHARACTER_FPS 12

/* The State structure stores a list of descriptive states in which an animated
 * object can be found, aka the different types of sprites you can have for a
 * given same object. */
typedef enum State State;
enum State
{
    MOVE,
    LIFT,
    PUSH,
    PULL,
    PUSH_SHIELD,
    MOVE_SHIELD,
    STILL
};

/* The animation structure is used to store a bunch of infos useful to set up an
 * object animation. */
typedef struct Animation Animation;
struct Animation
{
    State state; // The current state the object is in.
    Bool animated;

    /* array of infos about the frames of the animated object
     * [possible directions][State][max number of frames]
     * This array is NOT set if animation->animated is FALSE. */
    SDL_Rect frames[NW+1][MOVE_SHIELD+1][FPS];

    int total_frames;
    int current_frame;
    int framerate;
    int time; // previous tick (for changing frame)
};

/* This function initialize the Animation structure. A number of frames of 2 and
 * above sets the animated bool to TRUE and initialize the frames SDL_Rect
 * array. Frames must be a positive > 0 number. */
void init_animation(Animation *animation, int const frames);

/* This function is used to change the frame the Animation is currently in if
 * the object has passed it's framerate timing. The function also stores in the
 * Animation object the last time it has changed the frame, for
 * synchronisation.
 * The time_t type is not used as it's a long, and SDL uses Uint32... */
void check_animation_frame(Animation *animation, int const time);


#endif // define RPG_CHARACTERS
