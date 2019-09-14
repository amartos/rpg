#include "../errors.h"
#include "../try_throw_catch.h"
#include "../macros.h"
#include "../map.h"

#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>


static void printm(unsigned int** const map, unsigned int const maxx, unsigned int const maxy)
{
    unsigned int i, j;
    for (j=0;j<maxy;j++)
    {
        for (i=0;i<maxx;i++)
            if (map[j][i])
                printf("%04X ", map[j][i]);
	    else
                printf("     ");
	printf("\n");
    }
    printf("\n");
}


int main(int argc, char *argv[])
{
    Map map; init_map(&map, "test/test_map.map");
    printf("max: x %d, y %d\n", map.maxx, map.maxy);
    printf("BACKGROUND:\n"); printm(map.schematics[BACKGROUND], map.maxx, map.maxy);
    printf("FOREGROUND:\n"); printm(map.schematics[FOREGROUND], map.maxx, map.maxy);
    printf("COLLISIONS:\n"); printm(map.schematics[COLLISIONS], map.maxx, map.maxy);
    printf("COST:\n"); printm(map.schematics[COST], map.maxx, map.maxy);
    printf("WEATHER:\n"); printm(map.schematics[WEATHER], map.maxx, map.maxy);
}
