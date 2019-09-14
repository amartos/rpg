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

    coord.x = 0.0; coord.y = 0.0;
    printf("is colliding: (%f, %f) %d\n", coord.x, coord.y, is_colliding(coord, collisions));

    coord.x = 2.0; coord.y = 2.0;
    printf("is colliding: (%f, %f) %d\n", coord.x, coord.y, is_colliding(coord, collisions));

    coord.x = 0.3; coord.y = 0.7;
    printf("is colliding: (%f, %f) %d\n", coord.x, coord.y, is_colliding(coord, collisions));

    coord.x = 1.5; coord.y = 1.25;
    printf("is colliding: (%f, %f) %d\n", coord.x, coord.y, is_colliding(coord, collisions));

    printf("\n");

    coord.x = 1.0; coord.y = 3.0;
    coordb.x = 2.0; coordb.y = 2.0;
    printf("are corners colliding: (%f, %f) (%f, %f) %d\n", coord.x, coord.y, coordb.x, coordb.y, are_corners_colliding(coord, coordb, collisions));

    coordb.x = 2.0; coordb.y = 3.0;
    printf("are corners colliding: (%f, %f) (%f, %f) %d\n", coord.x, coord.y, coordb.x, coordb.y, are_corners_colliding(coord, coordb, collisions));

    printf("\n");

    coord.x = 2.0; coord.y = 2.0;
    printf("is out of map: (%f, %f) %d\n", coord.x, coord.y, is_out_of_map(coord, max));

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
