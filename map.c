#include "map.h"


// This function allocate memory for a double array of unsigned int of size
// [maxy][maxy],, initializes it and returns the adress.
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

    map->collisions = double_array_malloc(map->maxx, map->maxy);
    map->cost = double_array_malloc(map->maxx, map->maxy);
    map->weather = double_array_malloc(map->maxx, map->maxy);

    MALLOC(map->tiles, sizeof(unsigned int *) * MAX_LEVELS, MAP_MALLOC_FAILURE, NULL);
    for (l=0;l<MAX_LEVELS;l++)
        map->tiles[l] = double_array_malloc(map->maxx, map->maxy);
}

void init_map(Map *map, char map_name[])
{
    // usually the letter l is used for the loops in tiles level, but the DB
    // loops need the letter i.
    unsigned int x, y, i;
    char query[100] = {0};

    // The DB path is defined in the database module
    INIT_DB

    // Here we check the size (x and y) of the map in DB.
    // It is used for malloc.
    sprintf(query, "SELECT * FROM maps where name='%s';", map_name);
    QUERY_DB(query)
        map->maxx = GET_QUERY_INT(2) + 1;
        map->maxy = GET_QUERY_INT(3) + 1;
    END_QUERY

    map_malloc(map);

    // Here we fill the map with the infos from DB.
    sprintf(query, "SELECT * FROM %s;", map_name);
    QUERY_DB(query)
    {
        // For each tile in the map_name table:
        // x|y|collide|cost|weather|tiles0|tiles1...
        // tiles are idendified by a hex number between 0 and 0xFFFF and should
        // correspond to the id of the images table (see assets module).
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
        for (y=0;y<map->maxy;y++)
            free(map->tiles[l][y]);
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
