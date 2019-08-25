#ifndef RPG_CHARACTERS
#define RPG_CHARACTERS

#define SPRITES_WIDTH 32
#define SPRITES_HEIGHT 32

#include "errors.h"
#include "try_throw_catch.h"
#include "macros.h"

#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

// structures
typedef enum Face Face;
enum Face 
{
    LEFT, DOWN, UP, RIGHT
};

typedef enum State State;
enum State 
{
    MOVE, LIFT, PUSH, PULL, PUSH_SHIELD, MOVE_SHIELD
};

typedef struct Character Character;
struct Character
{
    SDL_Surface *sprite;
    State state;
    Face direction;
    SDL_Rect infos;
    int animated;
    int moving;
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
        const char sprite_path[],
        const int number_of_frames,
        const int moving,
        const int fps,
        const int velocity
        );
void free_character(Character *character);
void move_character(Character *character, const int direction, const int current_time);

#endif // define RPG_CHARACTERS
