#ifndef RPG_COORD
#define RPG_COORD

#include "errors.h"
#include "macros.h"

#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <math.h>


/* Maximum number of floors defined in each map.
 * This counts the ground + other floors.
 * Note that for isometric depth, more than 1 floor needs to be defined to make
 * walls. */
#define MAX_LEVELS 3

/* Order of the enum is set to match a clock round, from N to NW
 * (see the map structure comments for schema).
 * the order of the sprites on the asset img depends on this */
typedef enum Cardinals Cardinals;
enum Cardinals {N, NE, E, SE, S, SW, W, NW};

/* This structures describes coordinates on the map. Doubles are used for
 * precision. This allows for sub-tiling, tiles coord are recovered by
 * truncating the doubles before the floating point. */
typedef struct Coord Coord;
struct Coord
{
    double x;
    double y;
};

/* Function to initialize Coord struct.
 * Sets the values to 0.000 */
void init_coord(Coord *coord);

// This function resets the Coord's x and y values to 0.000
void reset_coord(Coord *coord);

// Returns a coord set which is the sum of the two others (x+x,y+y)
Coord sum_coord(Coord const a, Coord const b);

// Converts two ints into a returned Coord
Coord int_to_coord(unsigned int const x, unsigned int const y);

/* When given a Coord struct in which the doubles have numbers after the
 * floating point, it returns a Coord struct in which the doubles were
 * truncated before the floating point.
 * This is useful in a tiled system, as the clicks on the map give precise
 * coordinates, and this function convert theses into the corresponding tile
 * coord. */
Coord click_to_tile(Coord const click);

// Returns TRUE if both x and y are == 0.000
Bool is_coord_empty(Coord const coord);

// Returns TRUE if a == b
Bool is_same_coord(Coord const a, Coord const b);

/* If b is a tile's coords, and a is a sub-tile coord of b, then returns TRUE.
 * The reverse is also TRUE (a is tile and b subtile).
 * This works only if a or b is a tile's coord. */
Bool is_within_tile(Coord const a, Coord const b);

// Returns TRUE if goal is whithin a collision tile.
Bool is_colliding(Coord const goal, unsigned int** const collision_map, Coord const max_coord);

/* When doing a diagonal movement, at least one of the two common neighbours
 * squares between start and goal can be a colliding tile. Thus, the movement
 * pass through a corner, which seems not natural.
 * This function returns TRUE if there is a "corner" tile colliding. */
Bool are_corners_colliding(Coord const start, Coord const goal, unsigned int** const collision_map, Coord const max_coord);

// Returns TRUE if goal coord is out of map.
Bool is_out_of_map(Coord const goal, Coord const max_coord);

// Returns FALSE if position if not walkable.
Bool is_pos_legal(Coord const position, Coord const char_pos, Coord const max_coord, unsigned int** const collisions);

// Returns the cardinal value for the direction vector start -> goal.
Cardinals determine_direction(Coord const start, Coord const goal);

#endif // define RPG_COORD
