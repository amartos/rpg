#include "navigation_test.h"


static void printmap(Map const map)
{
    unsigned int x, y;
    unsigned int maxx = map.max.x, maxy = map.max.y;

    // max coord
    printf("max: x %f, y %f\n", map.max.x, map.max.y);
    printf("\n");
    
    // tiles
    for (unsigned int i=0; i<MAX_LEVELS;i++)
    {
        printf("LEVEL %d:\n", i);
        PRINTM(map.tiles[i][y][x]);
    }

    printf("COLLISIONS:\n"); PRINTM(map.collisions[y][x]);
    printf("COST:\n"); PRINTM(map.cost[y][x]);
    printf("WEATHER:\n"); PRINTM(map.weather[y][x]);
}

static void get_neighbours(Coord neighbours[8], Coord const current)
{
    neighbours[N].x = current.x;
    neighbours[N].y = current.y - 1;

    neighbours[NE].x = current.x + 1;
    neighbours[NE].y = current.y - 1;

    neighbours[E].x = current.x + 1;
    neighbours[E].y = current.y;

    neighbours[SE].x = current.x + 1;
    neighbours[SE].y = current.y + 1;

    neighbours[S].x = current.x;
    neighbours[S].y = current.y + 1;

    neighbours[SW].x = current.x - 1;
    neighbours[SW].y = current.y + 1;

    neighbours[W].x = current.x - 1;
    neighbours[W].y = current.y;

    neighbours[NW].x = current.x - 1;
    neighbours[NW].y = current.y - 1;
}

static void printcoord(Coord const coord)
{
    printf("(%f, %f)\n", coord.x, coord.y);
}

int main(int argc, char *argv[])
{
    printf("\n########## COORD ##########\n\n");

    // init_coord
    Coord c; init_coord(&c);
    printf("init: "); printcoord(c);
    printf("\n");

    // sum_coord
    Coord a,b; init_coord(&a); init_coord(&b);
    a.x = 7.0 ; b.x = 3.0; // sum 10
    a.y = 2.0 ; b.y = 12.0; // sum 14
    c = sum_coord(a, b);
    printf("sum:\n");
    printf("a:  "); printcoord(a);
    printf("b:  "); printcoord(b);
    printf("a+b:"); printcoord(c);
    printf("\n");

    // reset_coord
    printf("reset:\n");
    printf("before: "); printcoord(c);
    reset_coord(&c);
    printf("after:  "); printcoord(c);
    printf("\n");

    // int_to_coord
    c = int_to_coord(12, 37);
    printf("int to coord (12,37): "); printcoord(c);
    printf("\n");

    // click_to_tile
    a.x = 12.52; a.y = 37.98; c = click_to_tile(a);
    printf("click to tile:\n");
    printf("before: "); printcoord(a);
    printf("after:  "); printcoord(c);
    printf("\n");

    // is_coord_empty
    reset_coord(&a);
    printf("is coord empty ?\n");
    printf("is empty: %d ", is_coord_empty(a)); printcoord(a);
    printf("is empty: %d ", is_coord_empty(c)); printcoord(c);
    printf("\n");

    // is_same_coord
    a.x = 12.0; a.y = 5.3;
    b.x = 12.0; b.y = 5.3;
    c.x = 1.2; c.y = 5.7;
    printf("is same coord ?\n");
    printf("a: "); printcoord(a);
    printf("b: "); printcoord(b);
    printf("c: "); printcoord(c);
    printf("is same (a,b): %d\n", is_same_coord(a,b));
    printf("is same (a,c): %d\n", is_same_coord(a,c));
    printf("\n");
    
    // is_within_tile
    a.x = 12.64; a.y = 5.3;
    b.x = 13.0; b.y = 6.0;
    c.x = 12.0; c.y = 5.0;
    printf("is within tile ?\n");
    printf("a: "); printcoord(a);
    printf("b; "); printcoord(b);
    printf("c: "); printcoord(c);
    printf("is within tile (a,b): %d\n", is_within_tile(a,b));
    printf("is within tile (a,c): %d\n", is_within_tile(a,c));
    printf("\n");

    // determine_direction
    c.x = 2.0; c.y = 2.0;
    printf("determine direction\n");
    printf("start: "); printcoord(c);
    Coord neighbours[8];
    for (int i=0;i<8;i++)
        init_coord(&neighbours[i]);
    get_neighbours(neighbours, c);
    for (int i=0;i<8;i++)
    {
        printf("direction: %d, ", determine_direction(c, neighbours[i]));
        printcoord(neighbours[i]);
    }
    printf("\n");

    printf("\n########## EMPTY MAP ##########\n\n");

    // init_empty_map
    Map emap; init_empty_map(&emap, 3, 3); printmap(emap);

    printf("\n########## MAP ##########\n\n");

    // init_map from db
    Map map; init_map(&map, "unit_test"); printmap(map);

    printf("\n########## COORD & MAP ##########\n\n");

    // is_colliding
    a.x = 1.0; a.y = 1.0;
    c.x = 0.0; c.y = 1.0;
    printf("is colliding ?\n");
    printf("a: %d, ", is_colliding(a, map)); printcoord(a);
    printf("c: %d, ", is_colliding(c, map)); printcoord(c);
    printf("\n");

    // is_out_of_map
    a.x = 2.0; a.y = 2.0;
    b.x = -1.0; b.y = -1.0;
    c.x = 7.0; c.y = 8.0;
    printf("is out of map ?\n");
    printf("a: %d, ", is_out_of_map(a, map)); printcoord(a);
    printf("b: %d, ", is_out_of_map(b, map)); printcoord(b);
    printf("c: %d, ", is_out_of_map(c, map)); printcoord(c);
    printf("\n");

    // are_corners_colliding
    a.x = 3.0; a.y = 2.0;
    b.x = 3.0; b.y = 1.0;
    c.x = 2.0; c.y = 1.0;
    printf("are corners colliding ?\n");
    printf("start: "); printcoord(c);
    printf("goal 1: "); printcoord(a);
    printf("goal 2: "); printcoord(b);
    printf("corner collide (a,c): %d\n", are_corners_colliding(c, a, map));
    printf("corner collide (b,c): %d\n", are_corners_colliding(c, b, map));
    printf("\n");

    /* is_pos_legal
     * This function returns TRUE if the functions
     * - is_same_coord
     * - is_colliding
     * - is_out_of_map
     * all return FALSE.
     * There is thus no need to test it. */

    printf("\n");
    free_map(&map);
    free_map(&emap);
}
