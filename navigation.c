#include "navigation.h"


void init_coord(Coord *coord)
{
    coord->x = 0.0;
    coord->y = 0.0;
}

void reset_coord(Coord *coord)
{
    init_coord(coord);
}

Coord sum_coord(Coord const a, Coord const b)
{
    Coord sum; init_coord(&sum);
    sum.x = a.x + b.x;
    sum.y = a.y + b.y;
    return sum;
}

Coord int_to_coord(unsigned int const x, unsigned int const y)
{
    Coord coord; init_coord(&coord);
    coord.x = x; coord.y = y;
    return coord;
}

Coord click_to_tile(Coord const click)
{
    unsigned int x = click.x, y = click.y;
    Coord tile = int_to_coord(x, y);
    return tile;
}

Bool is_coord_empty(Coord const coord)
{
    Coord null; init_coord(&null);
    return is_same_coord(coord, null);
}

Bool is_same_coord(Coord const a, Coord const b)
{
    return is_same_double(a.x, b.x) && is_same_double(a.y, b.y);
}

Bool is_within_tile(Coord const a, Coord const b)
{
    /* to test if a is within b or reverse, we just test if they are from the
     * same tile. For this to work, one of them should be the tile coords. */
    Coord a_tile = click_to_tile(a);
    Coord b_tile = click_to_tile(b);
    return is_same_coord(a_tile, b_tile);
}

Bool is_colliding(Coord const goal, Map const map)
{
    unsigned int x = goal.x, y = goal.y;
    if(!is_out_of_map(goal, map))
        return convert_to_bool(map.collisions[y][x]);
    return TRUE;
}

Bool are_corners_colliding(
        Coord const start,
        Coord const goal,
        Map const map
        )
{
    /* 0 --------> max_coord.x - 1
     * | NW N NE
     * |  W . E
     * | SW S SE
     * v
     * max_coord.y - 1 */

    /* For this function to work we need to check if a corner is near the
     * diagonal path. This translates to checking if the common neighbours of
     * the start and goal squares are colliding (aka walls). */

    Cardinals direction = determine_direction(start, goal);

    /* N, S, W and E directions are not concerned by edge collision
     * The way the Struct is made makes these cardinals as even numbers */
    if (direction % 2 == 0)
        return FALSE;

    /* The maximus and decumanus vars are the Coord vars for the neighbours.
     * maximus is for the N/S axis (it is either the North neighbour or South
     * neighbour), decumanus for W/E (same, West or East).
     * see here for var names signification:
     * https://en.wikipedia.org/wiki/Cardo#Etymology */

    /* For the N/S axis, the neighbours' x coord will never change.
     * The y axis test checks if the neighbour is North or South. */
    Coord maximus; init_coord(&maximus);
    maximus.x = start.x;
    maximus.y = direction == NW || direction == NE ? start.y - 1 : start.y + 1;

    /* For the W/E axis, it is the neighbours' y coord that does not change.
     * The x axis test checks if the neighbour is West or East. */
    Coord decumanus; init_coord(&decumanus);
    decumanus.x = direction == NW || direction == SW ? start.x - 1 : start.x + 1;
    decumanus.y = start.y;

    Bool maximus_collide = is_colliding(maximus, map);
    Bool decumanus_collide = is_colliding(decumanus, map);

    return maximus_collide || decumanus_collide;
}

Bool is_out_of_map(Coord const goal, Map const map)
{
    return goal.x < 0 ||
           goal.y < 0 ||
           goal.x >= map.max.x ||
           goal.y >= map.max.y;
}

Bool is_pos_legal(
        Coord const position,
        Coord const char_pos,
        Map const map
        )
{
    return !is_same_coord(position, char_pos) &&
           !is_out_of_map(position, map) &&
           !is_colliding(position, map);
}

Cardinals determine_direction(Coord const start, Coord const goal)
{
    /* The 8 cardinals are placed at some specific points on a circle. Thus, it
     * is possible to calculate the direction by deducing the position of the
     * goal on the circle with start as the center. In math, the circle coords
     * are defined as usual, with the Y axis going up. */

    /* First, we need to calculate the angle between the position "start.x+1"
     * and goal with start as the edge. This is done by using atan2, which takes
     * an x and y coord in the math system. Here start coords are substracted
     * from goal as it is the center of the circle. atan2 returns an angle in
     * radius, converted in degrees by *180/PI. */
    double angle = degrees(atan2(goal.y - start.y, goal.x - start.x));

    /* The angle divided by 45 is a trick to get the cardinal corresponding
     * number. However, we need to convert from the math coord system to the
     * map coord system, by rotating by 90°. */
    int direction = round((90+angle) / 45);

    // The angle conversion gives -1 for the 8th value because of a -135° angle
    direction = direction < 0 ? 7 : direction;

    return direction;
}

/* This function allocate memory for a double array of unsigned int of size
 * [maxy][maxy],, initializes it and returns the adress. */
static unsigned int** double_array_malloc(
        unsigned int const maxx,
        unsigned int const maxy
        )
{
    unsigned int x, y, **submap;

    MALLOC(submap, sizeof(unsigned int *) * maxy, MAP_MALLOC_FAILURE, NULL);
    for(y=0;y<maxy;y++)
    {
        MALLOC(submap[y], sizeof(unsigned int) * maxx, MAP_MALLOC_FAILURE, NULL);
        for (x=0;x<maxx;x++)
            submap[y][x] = 0;
    }
    return submap;
}

// This function allocates memory to each array of the Map structure.
static void map_malloc(Map *map)
{
    unsigned int l;

    map->collisions = double_array_malloc((int)map->max.x, (int)map->max.y);
    map->cost = double_array_malloc((int)map->max.x, (int)map->max.y);
    map->weather = double_array_malloc((int)map->max.x, (int)map->max.y);

    MALLOC(map->tiles, sizeof(unsigned int *) * MAX_LEVELS, MAP_MALLOC_FAILURE, NULL);
    for (l=0;l<MAX_LEVELS;l++)
        map->tiles[l] = double_array_malloc((int)map->max.x, (int)map->max.y);
}

void init_map(Map *map, char map_name[])
{
    /* usually the letter l is used for the loops in tiles level, but the DB
     * loops need the letter i. */
    unsigned int x, y, i;
    char query[100] = {0};

    // The DB path is defined in the database module
    INIT_DB

    /* Here we check the size (x and y) of the map in DB.
     * It is used for malloc. */
    sprintf(query, "SELECT * FROM maps where name='%s';", map_name);
    QUERY_DB(query)
        map->max = int_to_coord(GET_QUERY_INT(2) + 1, GET_QUERY_INT(3) + 1);
    END_QUERY

    map_malloc(map);

    // Here we fill the map with the infos from DB.
    sprintf(query, "SELECT * FROM %s;", map_name);
    QUERY_DB(query)
    {
        /* For each tile in the map_name table:
         * x|y|collide|cost|weather|tiles0|tiles1...
         * tiles are idendified by a hex number between 0 and 0xFFFF and should
         * correspond to the id of the images table (see assets module). */
        x = GET_QUERY_INT(0);
        y = GET_QUERY_INT(1);
        map->collisions[y][x] = GET_QUERY_INT(2);
        map->cost[y][x] = GET_QUERY_INT(3);
        map->weather[y][x] = GET_QUERY_INT(4);
        for (i=0;i<MAX_LEVELS;i++)
            map->tiles[i][y][x] = GET_QUERY_INT(5+i);
    }
    END_QUERY
    CLOSE_DB
}

void free_map(Map *map)
{
    unsigned int l,y;
    for (l=0;l<MAX_LEVELS;l++)
    {
        for (y=0;y<map->max.y;y++)
            free(map->tiles[l][y]);
    }
    free(map->tiles);
    free(map->collisions);
    free(map->cost);
    free(map->weather);
}

void init_empty_map(Map *map, unsigned int const maxx, unsigned int const maxy)
{
    map->max = int_to_coord(maxx, maxy);
    map_malloc(map);
}
