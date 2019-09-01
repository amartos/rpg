#ifndef RPG_CHARACTERS
#define RPG_CHARACTERS

#include "errors.h"
#include "try_throw_catch.h"
#include "macros.h"
#include "images.h"
#include "movement.h"

#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>


typedef enum State State;
enum State 
{
    MOVE, LIFT, PUSH, PULL, PUSH_SHIELD, MOVE_SHIELD
};

typedef struct Character Character;
struct Character
{
    SDL_Surface *sprite;
    Coord position;
    Direction direction;
    Coord goal;
    Coord *path;
    unsigned int nodes;

    State state;
    Bool animated;
    MovementType movement_type;
    Bool moving;
    unsigned int number_of_frames;
    unsigned int velocity;
    SDL_Rect frames[4][6][30]; // possible positions, State, max number of frames
    unsigned int current_frame;
    unsigned int framerate;
    unsigned int previous_time;
};

// prototypes
void init_character(
        Character *character,
        SDL_Color const colors[],
        char const sprite_path[],
        unsigned int const number_of_frames,
        unsigned int const fps,
        unsigned int const velocity,
        Coord const start_position
        );
void free_character(Character *character);
void free_path(Character *character);

#endif // define RPG_CHARACTERS
