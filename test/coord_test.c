#include "../macros.h"
#include "../coord.h"

#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>


int main(int argc, char *argv[])
{
    Coord coord; init_coord(&coord);
    printf("init: (%f, %f)\n", coord.x, coord.y);

    printf("\n");

    coord.x = 1.0; coord.y = 1.0;
    printf("coord %f: (%f, %f)\n", 1.0, coord.x, coord.y);

    coord.x = 3.2; coord.y = 6.5;
    printf("coord %f: (%f, %f)\n", 3.2, coord.x, coord.y);

    coord.x = 5.75; coord.y = 1.36;
    printf("coord %f: (%f, %f)\n", 5.75, coord.x, coord.y);

    printf("\n");

    printf("cartesian: (%f, %f), ", coord.x, coord.y);
    coord = cartesian_to_isometric(coord);
    printf("isometric: (%f, %f), ", coord.x, coord.y);
    coord = isometric_to_cartesian(coord);
    printf("re-cartesian: (%f, %f)\n", coord.x, coord.y);

    printf("\n");

    coord = event_to_coord(745, 348);
    printf("event to coord: (%d, %d) => (%f, %f)\n", 745, 348, coord.x, coord.y);

    printf("\n");

    SDL_Rect rect = coord_to_isosdlrect(coord);
    printf("sdl_rect: x %d, y %d, w %d, h %d\n ", rect.x, rect.y, rect.w, rect.h);

    printf("\n");

    Coord coordb; init_coord(&coordb);
    printf("same: (%f, %f) (%f, %f) = %d\n", coord.x, coord.y, coordb.x, coordb.y, is_same_coord(coord, coordb));
    coordb = coord;
    printf("same: (%f, %f) (%f, %f) = %d\n", coord.x, coord.y, coordb.x, coordb.y, is_same_coord(coord, coordb));

    printf("\n");

    reset_coord(&coord);
    printf("reset: (%f, %f)\n", coord.x, coord.y);

    printf("\n");

    coord.x = 5.0; coord.y = 4.0;
    coordb.x = 5.125; coordb.y = 4.975;
    printf("within tile: (%f, %f) (%f, %f) = %d\n", coord.x, coord.y, coordb.x, coordb.y, is_within_tile(coord, coordb));

    coordb.x = 5.0; coordb.y = 5.0;
    printf("within tile: (%f, %f) (%f, %f) = %d\n", coord.x, coord.y, coordb.x, coordb.y, is_within_tile(coord, coordb));

    printf("\n");

    coord.x = 5.0; coord.y = 5.0;

    reset_coord(&coordb); coordb.x = 5.0; coordb.y = 4.0;
    printf("direction  N %d: %d (%f, %f) (%f, %f)\n", N, determine_direction(coord, coordb), coord.x, coord.y, coordb.x, coordb.y);

    reset_coord(&coordb); coordb.x = 6.0; coordb.y = 4.0;
    printf("direction NE %d: %d (%f, %f) (%f, %f)\n", NE, determine_direction(coord, coordb), coord.x, coord.y, coordb.x, coordb.y);

    reset_coord(&coordb); coordb.x = 6.0; coordb.y = 5.0;
    printf("direction  E %d: %d (%f, %f) (%f, %f)\n", E, determine_direction(coord, coordb), coord.x, coord.y, coordb.x, coordb.y);

    reset_coord(&coordb); coordb.x = 6.0; coordb.y = 6.0;
    printf("direction SE %d: %d (%f, %f) (%f, %f)\n", SE, determine_direction(coord, coordb), coord.x, coord.y, coordb.x, coordb.y);

    reset_coord(&coordb); coordb.x = 5.0; coordb.y = 6.0;
    printf("direction  S %d: %d (%f, %f) (%f, %f)\n", S, determine_direction(coord, coordb), coord.x, coord.y, coordb.x, coordb.y);

    reset_coord(&coordb); coordb.x = 4.0; coordb.y = 6.0;
    printf("direction SW %d: %d (%f, %f) (%f, %f)\n", SW, determine_direction(coord, coordb), coord.x, coord.y, coordb.x, coordb.y);

    reset_coord(&coordb); coordb.x = 4.0; coordb.y = 5.0;
    printf("direction  W %d: %d (%f, %f) (%f, %f)\n", W, determine_direction(coord, coordb), coord.x, coord.y, coordb.x, coordb.y);

    reset_coord(&coordb); coordb.x = 4.0; coordb.y = 4.0;
    printf("direction NW %d: %d (%f, %f) (%f, %f)\n", NW, determine_direction(coord, coordb), coord.x, coord.y, coordb.x, coordb.y);

    printf("\n");

    Coord max; init_coord(&max);
    max.x = 4.0; max.y = 4.0;
    unsigned int i, j;
    // y and x, line and columns
    unsigned int map[5][5] = 
    {
        {1, 1, 1, 1, 1},
        {1, 0, 0, 0, 1},
        {1, 1, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 1, 1, 1, 1}
    };
    printf("map:\n");
    for (j=0;j<5;j++)
    {
        for (i=0;i<5;i++)
            printf("%d", map[j][i]);
        printf("\n");
    }
    printf("\n");

    unsigned int** collisions;
    collisions = (unsigned int**)malloc(sizeof(unsigned int**)*5);
    if (collisions == NULL)
        exit(EXIT_FAILURE);
    for (i=0;i<5;i++)
    {
        collisions[i] = (unsigned int*)malloc(sizeof(unsigned int*)*5);
	if (collisions[i] == NULL)
            exit(EXIT_FAILURE);
    }
    for (i=0;i<5;i++)
        for (j=0;j<5;j++)
            collisions[i][j] = map[i][j];

#define POS_TEST(x,y,m) \
    printf("(%f, %f) ", x, y); printf(m); \
    printf("coll: %d, ", is_colliding(coordb, collisions, max));\
    printf("out: %d, ", is_out_of_map(coordb, max));\
    printf("same: %d, ", is_same_coord(coord, coordb));\
    printf("is legal: %d\n", is_pos_legal(coordb, coord, max, collisions));

    coord.x = 3.0; coord.y = 1.0; printf("char_pos: (%f, %f) ; pos:\n", coord.x, coord.y);
    coordb.x = 1.0; coordb.y = 1.0; POS_TEST(coordb.x, coord.y, "legal  : ")
    coordb.x = 0.0; coordb.y = 0.0; POS_TEST(coordb.x, coord.y, "collide: ")
    coordb.x = 6.0; coordb.y = 6.0; POS_TEST(coordb.x, coord.y, "out    : ")
    coordb.x = 3.0; coordb.y = 1.0; POS_TEST(coordb.x, coord.y, "same   : ")

    printf("\n");

    coord.x = 1.0; coord.y = 3.0;
    coordb.x = 2.0; coordb.y = 2.0;
    printf("are corners colliding: (%f, %f) (%f, %f) %d\n", coord.x, coord.y, coordb.x, coordb.y, are_corners_colliding(coord, coordb, collisions, max));

    coordb.x = 2.0; coordb.y = 3.0;
    printf("are corners colliding: (%f, %f) (%f, %f) %d\n", coord.x, coord.y, coordb.x, coordb.y, are_corners_colliding(coord, coordb, collisions, max));

    printf("\n");

    coord.x = 0.0; coord.y = 0.0;
    printf("is out of map: (%f, %f) %d\n", coord.x, coord.y, is_out_of_map(coord, max));

    coord.x = -1.0; coord.y = 0.0;
    printf("is out of map: (%f, %f) %d\n", coord.x, coord.y, is_out_of_map(coord, max));

    coord.x = 0.0; coord.y = -1.0;
    printf("is out of map: (%f, %f) %d\n", coord.x, coord.y, is_out_of_map(coord, max));

    coord.x = -0.25; coord.y = 0.0;
    printf("is out of map: (%f, %f) %d\n", coord.x, coord.y, is_out_of_map(coord, max));

    coord.x = 0.0; coord.y = -0.25;
    printf("is out of map: (%f, %f) %d\n", coord.x, coord.y, is_out_of_map(coord, max));

    coord.x = -1.0; coord.y = -2.0;
    printf("is out of map: (%f, %f) %d\n", coord.x, coord.y, is_out_of_map(coord, max));

    coord.x = 5.0; coord.y = 4.0;
    printf("is out of map: (%f, %f) %d\n", coord.x, coord.y, is_out_of_map(coord, max));

    coord.x = 4.0; coord.y = 5.0;
    printf("is out of map: (%f, %f) %d\n", coord.x, coord.y, is_out_of_map(coord, max));

    coord.x = 5.0; coord.y = 5.0;
    printf("is out of map: (%f, %f) %d\n", coord.x, coord.y, is_out_of_map(coord, max));

    coord.x = 4.25; coord.y = 4.0;
    printf("is out of map: (%f, %f) %d\n", coord.x, coord.y, is_out_of_map(coord, max));

    coord.x = 4.0; coord.y = 4.25;
    printf("is out of map: (%f, %f) %d\n", coord.x, coord.y, is_out_of_map(coord, max));

    coord.x = 4.25; coord.y = 4.25;
    printf("is out of map: (%f, %f) %d\n", coord.x, coord.y, is_out_of_map(coord, max));

    for (i=0;i<5;i++)
        free(collisions[i]);
    free(collisions);
}
