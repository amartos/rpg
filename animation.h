#ifndef RPG_CHARACTERS
#define RPG_CHARACTERS

#include "errors.h"
#include "try_throw_catch.h"
#include "macros.h"
#include "images.h"
#include "coord.h"
#include "movement.h"
#include "map.h"

#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

// structures
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

typedef struct Animation Animation;
struct Animation
{
    State state;
    Bool animated;
    // possible positions, State, max number of frames
    SDL_Rect frames[NW+1][MOVE_SHIELD+1][FPS];
    unsigned int total_frames;
    unsigned int current_frame;
    unsigned int framerate;
    unsigned int time; // previous tick (for changing frame)
};

typedef struct AnimatedObject AnimatedObject;
struct AnimatedObject
{
    unsigned int id;
    Animation animation;
    Movement movement;
};

// prototypes
void change_animobj_formation(AnimatedObject objects[], unsigned int max, Deployment formation);
void check_animobj_frame(Animation *animation, unsigned int const time);
void make_animobj_move(
        AnimatedObject objects[],
        unsigned int max,
        Map const map,
        unsigned int const time,
        Bool paused
        );
// AnimatedObject initialization. For now the values are manually set, but in the
// future all will depend on the id
void init_animobj(AnimatedObject *object);

#endif // define RPG_CHARACTERS
