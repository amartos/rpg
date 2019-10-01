#include "map.h"


void map_malloc(Map *map)
{
    unsigned int i,j,t;
    // TODO: malloc needs to be better checked
    TRY
    {
        map->collisions = malloc(sizeof(unsigned int *) * map->maxy);
        map->cost = malloc(sizeof(unsigned int *) * map->maxy);
        map->weather = malloc(sizeof(unsigned int *) * map->maxy);
        if (
                map->collisions == NULL ||
                map->cost == NULL ||
                map->weather == NULL
            )
            THROW(MAP_MALLOC_FAILURE);
        for(j=0;j<map->maxy;j++)
        {
            map->collisions[j] = malloc(sizeof(unsigned int) * map->maxx);
            map->cost[j] = malloc(sizeof(unsigned int) * map->maxx);
            map->weather[j] = malloc(sizeof(unsigned int) * map->maxx);
            if (
                    map->collisions[j] == NULL ||
                    map->cost[j] ==NULL ||
                    map->weather[j] == NULL
                )
                THROW(MAP_MALLOC_FAILURE);

            for (i=0;i<map->maxx;i++)
            {
                map->collisions[j][i] = 0;
                map->cost[j][i] = 0;
                map->weather[j][i] = 0;
            }
        }

        map->tiles = malloc(sizeof(unsigned int *) * (MAX_LEVELS));
        if (map->tiles == NULL)
            THROW(MAP_MALLOC_FAILURE);
        for (t=0;t<MAX_LEVELS;t++)
        {
            map->tiles[t] = malloc(sizeof(unsigned int *) * map->maxy);
            if (map->tiles[t] == NULL)
                THROW(MAP_MALLOC_FAILURE);

            for(j=0;j<map->maxy;j++)
            {
                map->tiles[t][j] = malloc(sizeof(unsigned int) * map->maxx);
                if (map->tiles[t][j] == NULL)
                    THROW(MAP_MALLOC_FAILURE);

                for (i=0;i<map->maxx;i++)
                    map->tiles[t][j][i] = 0;
            }
        }
    }
    CATCH(MAP_MALLOC_FAILURE)
    {
        logger(MAP_MALLOC_FAILURE, "oups");
        exit(EXIT_FAILURE);
    }
    ETRY;
}

void init_map(Map *map)
{
    unsigned int x, y, i;
    INIT_DB

    QUERY_DB("SELECT MAX(x) FROM test_map;")
        map->maxx = GET_QUERY_INT(0) + 1;
    END_QUERY
    QUERY_DB("SELECT MAX(y) FROM test_map;")
        map->maxy = GET_QUERY_INT(0) + 1;
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
