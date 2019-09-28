#ifndef RPG_MOVEMENT
#define RPG_MOVEMENT

#include "errors.h"
#include "try_throw_catch.h"
#include "macros.h"

#include "coord.h"
#include "pathfinding.h"

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
    double velocity; // in units
};

// prototypes
void init_movement(Movement *movement);
void deploy(
        Coord *position,
        Cardinals const direction,
        Deployment const formation,
        unsigned int const char_number
        );
void move(
        Movement *movement,
        Coord const max_coord,
        unsigned int** const collision_map,
        unsigned int** const cost_map
        );
void fire_movement(Movement *movement, MovementType const movement_type);
void stop_movement(Movement *movement);

#endif // define RPG_MOVEMENT
