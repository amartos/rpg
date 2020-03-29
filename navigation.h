#ifndef RPG_NAVIGATION
#define RPG_NAVIGATION

#include "errors.h"
#include "macros.h"
#include "database.h"

#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
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

/* The main Map structure.
 * The map representation is:
 * 0 --------> map.max.x - 1
 * | NW N NE
 * |  W . E
 * | SW S SE
 * v
 * map.max.y - 1 */
typedef struct Map Map;
struct Map
{
    /* max number of tiles for x and y
     * the minimum is always == 0 */
    Coord max;
    int ***tiles; // image id for each tile
    /* This array is, for now, used as a Bool to determine collision (1 is
     * colliding, 0 is not). In the future, different types of collision might
     * be defined, thus needing ids instead of Bool.
     * Moreover, using the same type as the others arrays of the Map simplifies
     * code for malloc. */
    int **collisions; // Bool 
    int **cost; // movement cost for each tile
    /* Weather ID for each tile. This might be modified in the future to
     * integrate a third dimension (or more). */
    int **weather;
};

/* Function to initialize Coord struct.
 * Sets the values to 0.000 */
void init_coord(Coord *coord);

// This function resets the Coord's x and y values to 0.000
void reset_coord(Coord *coord);

// Returns a coord set which is the sum of the two others (x+x,y+y)
Coord sum_coord(Coord const a, Coord const b);

// Converts two ints into a returned Coord
Coord int_to_coord(int const x, int const y);

// Converts a set of coord into two ints
void coord_to_int(Coord const coord, int *x, int *y);

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
Bool is_colliding(Coord const goal, Map const map);

/* When doing a diagonal movement, at least one of the two common neighbours
 * squares between start and goal can be a colliding tile. Thus, the movement
 * pass through a corner, which seems not natural.
 * This function returns TRUE if there is a "corner" tile colliding. */
Bool are_corners_colliding(Coord const start, Coord const goal, Map const map);

// Returns TRUE if goal coord is out of map.
Bool is_out_of_map(Coord const goal, Map const map);

// Returns FALSE if position if not walkable.
Bool is_pos_legal(Coord const position, Coord const char_pos, Map const map);

// Returns the cardinal value for the direction vector start -> goal.
Cardinals determine_direction(Coord const start, Coord const goal);

/* The main map initialization function. This NEEDS the map to be fully defined in
 * the DB which path is defined in the database module. The map_name is used as
 * a table name, and an entry key in the maps table.
 * To know more about the DB tables, see the map_crator module. */
void init_map(Map *map, char const map_name[]);

/* This function is used with the map_creator tool, to initialize an empty map
 * of defined size. This is used only for malloc, as the map does not need to be
 * in the DB. */
void init_empty_map(Map *map, int const maxx, int const maxy);

// This function frees all arrays of the Map struct.
void free_map(Map *map);

#endif // define RPG_NAVIGATION
