#ifndef RPG_MOVEMENT
#define RPG_MOVEMENT

#include "macros.h"

#include <stdio.h>
#include <stdlib.h>

// structures
typedef struct Coord Coord;
struct Coord
{
    unsigned int x;
    unsigned int y;
};

typedef enum MovementType MovementType;
enum MovementType {WALK, TELEPORT};

typedef enum Direction Direction;
// this order depends on the sprite order
enum Direction{LEFT, DOWN, UP, RIGHT};

typedef enum Cardinals Cardinals;
enum Cardinals {N, NE, E, SE, S, SW, W, NW};

// prototypes
void init_coord(Coord *coord);
void move(
        Coord *start,
        Coord *goal,
        MovementType const type,
        Coord const max_coord,
        unsigned int** const collision_map,
        int const velocity
        );

#endif // define RPG_MOVEMENT
