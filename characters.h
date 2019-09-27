#ifndef RPG_CHARACTERS
#define RPG_CHARACTERS

#define MAX_CHARACTERS 4 // max of team

#include "errors.h"
#include "try_throw_catch.h"
#include "macros.h"
#include "images.h"
#include "coord.h"
#include "movement.h"

#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
// this should be removed at some point
#include <SDL2/SDL2_rotozoom.h>

// structures
// This structure describes the different states a character object can be
// found in, mainly related to the sprites.
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

// this give infos on what to show on screen for Character objects
typedef struct OnScreen OnScreen;
struct OnScreen
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


// This describes a Character type object, aka all what can be described as a
// living being
typedef struct Character Character;
struct Character
{
    unsigned int id;
    OnScreen on_screen;
    Movement movement;
};

// prototypes
// Character initialization. For now the values are manually set, but in the
// future all will depend on the id
void init_character(
        Character *character,
        unsigned int const id,
        Coord const start_position,
        Deployment const formation
        );

#endif // define RPG_CHARACTERS
