#ifndef RPG_MOVEMENT
#define RPG_MOVEMENT

#define MAX_PATH_NODES 100

#include "errors.h"
#include "try_throw_catch.h"
#include "macros.h"

#include "coord.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// structures
// This struct is used to describe different formations that the characters'
// team can apply. Circle is actually a half-circle, given the MAX_CHAR value
typedef enum Deployment Deployment;
enum Deployment {
    NONE,
    LINE,
    SQUARE,
    TRIANGLE,
    CIRCLE
};

// This structure is used by the movement module to manage different ways of
// moving the characters
typedef enum MovementType MovementType;
enum MovementType {WALK, PATH, TELEPORT};

// This structure is used by objects to describe their map & movement infos
typedef struct Movement Movement;
struct Movement
{
    Coord position;
    Deployment formation;
    Bool moving;
    Cardinals direction;
    Coord path[MAX_PATH_NODES]; // array of nodes
    unsigned int current_node;
    MovementType movement_type;
    unsigned int velocity; // in pixels
};

// prototypes
void formation_offsetting(
        Coord *position,
        unsigned int const char_number,
        Deployment const deployment
        );
void move(Movement *movement, Coord const max_coord, unsigned int** const collision_map);

#endif // define RPG_MOVEMENT
