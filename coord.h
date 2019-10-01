#ifndef RPG_COORD
#define RPG_COORD

#include "macros.h"

#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <math.h>


// structures
// the order of the sprites on the asset img depends on this
typedef enum Cardinals Cardinals;
enum Cardinals {N, NE, E, SE, S, SW, W, NW};

typedef struct Coord Coord;
struct Coord
{
    double x;
    double y;
};

// prototypes
void init_coord(Coord *coord);
void reset_coord(Coord *coord);
Coord isometric_to_cartesian(Coord const isometric);
Coord cartesian_to_isometric(Coord const cartesian);
Coord event_to_coord(Sint32 x, Sint32 y);
SDL_Rect coord_to_isosdlrect(Coord const coord);
Bool is_same_coord(Coord const a, Coord const b);
Bool is_within_tile(Coord const a, Coord const b);
Bool is_colliding(Coord const goal, unsigned int** const collision_map);
Bool are_corners_colliding(Coord const start, Coord const goal, unsigned int** const collision_map);
Bool is_out_of_map(Coord const goal, Coord const max_coord);
Bool is_pos_legal(Coord const position, Coord const char_pos, Coord const max_coord, unsigned int** const collisions);
Cardinals determine_direction(Coord const start, Coord const goal);

#endif // define RPG_COORD
