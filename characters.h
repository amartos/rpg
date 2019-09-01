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
    SDL_Rect infos;

    Coord position;
    Direction direction;
    Coord goal;
    Coord *path;
    int nodes;

    State state;
    Bool animated;
    Bool moving;
    int number_of_frames;
    int velocity;
    SDL_Rect frames[4][6][30]; // possible positions, State, max number of frames
    int current_frame;
    int framerate;
    int previous_time;
};

// prototypes
void init_character(
        Character *character,
        SDL_Color colors[],
        const char sprite_path[],
        const int number_of_frames,
        const Bool moving,
        const int fps,
        const int velocity,
        const Coord start_position
        );
void free_character(Character *character);

#endif // define RPG_CHARACTERS
