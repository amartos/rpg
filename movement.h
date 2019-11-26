#ifndef RPG_MOVEMENT
#define RPG_MOVEMENT

#include "errors.h"
#include "macros.h"

#include "coord.h"
#include "pathfinding.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* structures
 * This struct is used to describe different formations that the characters'
 * team can apply. Circle is actually a half-circle, given the MAX_CHAR value */
typedef enum Deployment Deployment;
enum Deployment {
    NONE,
    LINE,
    SQUARE,
    TRIANGLE,
    CIRCLE
};

/* This structure is used by the movement module to manage different ways of
 * moving the characters. The difference between PATH and WALK is that PATH
 * triggers pathfinding, and WALK make the character walk the path found. */
typedef enum MovementType MovementType;
enum MovementType {WALK, PATH, TELEPORT};

// This structure is used by objects to describe their map & movement infos
typedef struct Movement Movement;
struct Movement
{
    Coord position;
    /* The arrangement of characters' positions can be done in many ways. These
     * variables describe the type of "formation" (as in military group
     * arrangement) they are in and the offset to apply to positions to obtain
     * this formation. */
    Deployment formation;
    Coord formation_offset;
    Bool moving;
    Cardinals direction; // where the character looks to
    Coord path[MAX_PATH_NODES]; // array of coords to go through when moving
    unsigned int current_node; // index of current node from path to go to
    MovementType movement_type; // trigger for Movement mechanics
    double velocity; // movement speed in pixels
};

// initialize the Movement structure
void init_movement(Movement *movement);

/* this function sets the formation in the Movement structure and fires movement
 * to make the characters move to their new formation. */
void change_formation(Movement *movement, Deployment formation);

/* this function apply the calculated offset to the given position, depending on
 * the formation and the character. The character 0 is treated as the main
 * character, and its position is never offsetted (it becomes the center of the
 * formation). */
void deploy(Coord *position, Movement *movement, unsigned int const char_number);

/* this makes the characters move depending on the data stored in structure. If
 * the movement type in *movement is PATH, it triggers the pathfinding to the
 * node path[0]. Else, it either teleports the characters to the position, or
 * make them walk through the nodes array. When they reach their goal (or if
 * there is none), the movement stops. */
void move(
        Movement *movement,
        Coord const max_coord,
        unsigned int** const collision_map,
        unsigned int** const cost_map
        );

// Triggers character movements and set the bool moving to TRUE
void fire_movement(Movement *movement, MovementType const movement_type);

/* Stops the characters' movement and reset some of the vars in the Movement
 * structure (moving bool, path array, etc). */
void stop_movement(Movement *movement);

#endif // define RPG_MOVEMENT
