#include "map.h"


void map_malloc(Map *map)
{
    unsigned int i,j,t;
    size_t size;

    size = sizeof(unsigned int *) * map->maxy;
    MALLOC(map->collisions, size, MAP_MALLOC_FAILURE, NULL);
    MALLOC(map->cost, size, MAP_MALLOC_FAILURE, NULL);
    MALLOC(map->weather, size, MAP_MALLOC_FAILURE, NULL);
    for(j=0;j<map->maxy;j++)
    {
        size = sizeof(unsigned int) * map->maxx;
        MALLOC(map->collisions[j], size, MAP_MALLOC_FAILURE, NULL);
        MALLOC(map->cost[j], size, MAP_MALLOC_FAILURE, NULL);
        MALLOC(map->weather[j], size, MAP_MALLOC_FAILURE, NULL);

        for (i=0;i<map->maxx;i++)
        {
            map->collisions[j][i] = 0;
            map->cost[j][i] = 0;
            map->weather[j][i] = 0;
        }
    }

    size = sizeof(unsigned int *) * MAX_LEVELS;
    MALLOC(map->tiles, size, MAP_MALLOC_FAILURE, NULL);
    for (t=0;t<MAX_LEVELS;t++)
    {
        size = sizeof(unsigned int *) * map->maxy;
        MALLOC(map->tiles[t], size, MAP_MALLOC_FAILURE, NULL);
        for(j=0;j<map->maxy;j++)
        {
            size = sizeof(unsigned int) * map->maxx;
            MALLOC(map->tiles[t][j], size, MAP_MALLOC_FAILURE, NULL);
            for (i=0;i<map->maxx;i++)
                map->tiles[t][j][i] = 0;
        }
    }
}

void init_map(Map *map)
{
    unsigned int x, y, i;
    INIT_DB

    QUERY_DB("SELECT * FROM maps where name='test_map';")
        map->maxx = GET_QUERY_INT(2) + 1;
        map->maxy = GET_QUERY_INT(3) + 1;
    END_QUERY

    map_malloc(map);

    QUERY_DB("SELECT * FROM test_map;")
    {
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
    unsigned int t,j;
    for (t=0;t<MAX_LEVELS;t++)
    {
        for (j=0;j<map->maxy;j++)
            free(map->tiles[t][j]);
    }
    free(map->tiles);
    free(map->collisions);
    free(map->cost);
    free(map->weather);
}

void init_empty_map(Map *map, unsigned int const maxx, unsigned int const maxy)
{
    map->maxx = maxx, map->maxy = maxy;
    map_malloc(map);
}
