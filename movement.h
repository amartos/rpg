#ifndef RPG_MOVEMENT
#define RPG_MOVEMENT

#include "errors.h"
#include "try_throw_catch.h"
#include "macros.h"

#include <stdio.h>
#include <stdlib.h>


// structures
typedef struct Coord Coord;
struct Coord
{
    unsigned int x;
    unsigned int y;
    int ox; // offset
    int oy;
};

typedef enum MovementType MovementType;
enum MovementType {WALK, PATH, TELEPORT};

typedef enum Direction Direction;
// this order depends on the sprite order
enum Direction{LEFT, DOWN, UP, RIGHT, NONE};

typedef struct DirectionVector DirectionVector;
struct DirectionVector
{
    Direction x;
    Direction y;
};

typedef enum Cardinals Cardinals;
enum Cardinals {N, S, E, W, NE, NW, SE, SW};

typedef enum Deployment Deployment;
enum Deployment {LINE, SQUARE, TRIANGLE, CIRCLE};

// prototypes
void init_coord(Coord *coord);
Coord offsetting(Coord const position);
void get_formation_offset(
        Coord *position,
        unsigned int const char_number,
        Deployment const deployment
        );
Bool is_same_coord(Coord const a, Coord const b);
unsigned int find_path(
        Coord *(*path),
        Coord const start,
        Coord goal,
        unsigned int const velocity,
        Coord const max_coord,
        unsigned int** const collision_map,
        unsigned int** const movement_cost_map
        );
Direction move(
        Coord *start,
        Coord const goal,
        MovementType const type,
        Coord const max_coord,
        unsigned int** const collision_map,
        unsigned int const velocity
        );

#endif // define RPG_MOVEMENT
