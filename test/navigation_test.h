#ifndef RPG_NAVIGATION_TEST
#define RPG_NAVIGATION_TEST

#include "../navigation.h"

#include <stdio.h>
#include <stdlib.h>

#define MAP_LIMIT for (x=0;x<maxx;x++) {printf("-");} printf("\n");
#define PRINTM(m) \
        MAP_LIMIT \
        for (y=0;y<maxy;y++) \
        { \
            for (x=0;x<maxx;x++) \
                if (m) \
                    printf("%04X ", m); \
                else \
                    printf("     "); \
            printf("\n"); \
        } \
        MAP_LIMIT

#endif // define RPG_NAVIGATION_TEST
