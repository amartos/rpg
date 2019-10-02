#include "../errors.h"
#include "../try_throw_catch.h"
#include "../macros.h"
#include "../map.h"

#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

#define MAP_LIMIT for (i=0;i<maxx;i++) {printf("-");} printf("\n");

static void printm(unsigned int** const map, unsigned int const maxx, unsigned int const maxy)
{
    unsigned int i, j;

    MAP_LIMIT
    for (j=0;j<maxy;j++)
    {
        for (i=0;i<maxx;i++)
            if (map[j][i])
                printf("%04X ", map[j][i]);
	    else
                printf("     ");
	printf("\n");
    }
    MAP_LIMIT
    printf("\n");
}


int main(int argc, char *argv[])
{
    Map map; init_map(&map);
    printf("max: x %d, y %d\n", map.maxx, map.maxy);
    printf("LEVEL 0:\n"); printm(map.tiles[0], map.maxx, map.maxy);
    printf("LEVEL 1:\n"); printm(map.tiles[1], map.maxx, map.maxy);
    printf("COLLISIONS:\n"); printm(map.collisions, map.maxx, map.maxy);
    printf("COST:\n"); printm(map.cost, map.maxx, map.maxy);
    printf("WEATHER:\n"); printm(map.weather, map.maxx, map.maxy);
}
