#ifndef RPG_CHARACTERS
#define RPG_CHARACTERS

#define SPRITES_WIDTH 32
#define SPRITES_HEIGHT 32
#define COLLISION_BOX_WIDTH 24
#define COLLISION_BOX_HEIGHT 4
#define COLLISION_BOX_OFFSET_X 4
#define COLLISION_BOX_OFFSET_Y 28

#include "errors.h"
#include "try_throw_catch.h"
#include "macros.h"
#include "map.h"
#include "images.h"

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
    SDL_Rect collision_box;
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
        SDL_Color colors[],
        const char sprite_path[],
        const int number_of_frames,
        const int moving,
        const int fps,
        const int velocity
        );
void free_character(Character *character);
void move_character(Character *character, const int direction, const int current_time, const Map map);
void place_character(Character *character, const int x, const int y);

#endif // define RPG_CHARACTERS
