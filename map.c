#include "map.h"


static void map_malloc(Map *map)
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

void init_map(Map *map, char const map_path[])
{
    FILE *map_file = NULL;// FILE *converter_file = NULL;
    unsigned int i = 0, j = 0, offset = 0;
    char line[MAX_SIZE_LINE] = {0}, *data = NULL;
    unsigned int map_type = 0;

    TRY
    {
        map_file = fopen(map_path, "r");
        if (map_file == NULL)
            THROW(MAP_FILE_LOADING_FAILURE);
    }
    CATCH(MAP_FILE_LOADING_FAILURE)
    {
        char error_msg[200] = {0};
        sprintf(error_msg, "could not open map file at %s", map_path);
        logger(MAP_FILE_LOADING_FAILURE, error_msg);
        exit(EXIT_FAILURE);
    }
    CATCH(CONVERTER_FILE_LOADING_FAILURE)
    {
        logger(CONVERTER_FILE_LOADING_FAILURE, "could not open converter file");
        exit(EXIT_FAILURE);
    }
    ETRY;

    // get map infos
    while (fgets(line, MAX_SIZE_LINE, map_file) != NULL)
    {
        if (!strcmp(line, "# infos\n"))
            break;
    }
    fgets(line, MAX_SIZE_LINE, map_file); // empty line

    // maxx and maxy
    fgets(line, MAX_SIZE_LINE, map_file);
    data = line; sscanf(data, "x: %d", &map->maxx);
    fgets(line, MAX_SIZE_LINE, map_file);
    data = line; sscanf(data, "y: %d", &map->maxy);
    rewind(map_file);

    map_malloc(map);

    // This is a nasty piece of code as it supposes that the map files are all
    // constructed EXACTLY the same way, and that all the map lines are of the
    // same length (a multiple of 4 digits hex numbers + 1 space). It is too 
    // much constraint, but it's a start. If RLE compression is implemented, 
    // this code will definitely need to change
    while (fgets(line, MAX_SIZE_LINE, map_file) != NULL)
    {
        if (!strcmp(line, "# level0\n"))
        {
            map_type = 0;
            j = 0;
            offset = 5;
        }
        else if (!strcmp(line, "# level1\n"))
        {
            map_type = 1;
            j = 0;
            offset = 5;
        }
        else if (!strcmp(line, "# collisions\n"))
        {
            map_type = 10;
            j = 0;
            offset = 2;
        }
        else if (!strcmp(line, "# weather\n"))
        {
            map_type = 11;
            j = 0;
            offset = 5;
        }
        else if (!strcmp(line, "# movement cost\n"))
        {
            map_type = 12;
            j = 0;
            offset = 2;
        }
        else if (!strcmp(line, "\n"))
            continue;
        else
        {
            data = line;
            switch (map_type)
            {
                case 1:
                    for (i=0;i<map->maxx;i++)
                    {
                        sscanf(data, "%X ", &map->tiles[1][j][i]);
                        data += offset;
                    }
                    break;
                case 0:
                    for (i=0;i<map->maxx;i++)
                    {
                        sscanf(data, "%X ", &map->tiles[0][j][i]);
                        data += offset;
                    }
                    break;
                case 10:
                    for (i=0;i<map->maxx;i++)
                    {
                        sscanf(data, "%d ", &map->collisions[j][i]);
                        data += offset;
                    }
                    break;
                case 11:
                    for (i=0;i<map->maxx;i++)
                    {
                        sscanf(data, "%X ", &map->weather[j][i]);
                        data += offset;
                    }
                    break;
                case 12:
                    for (i=0;i<map->maxx;i++)
                    {
                        sscanf(data, "%X ", &map->cost[j][i]);
                        data += offset;
                    }
                    break;
            }
            j++;
        }
    }

    fclose(map_file);
}

void free_map(Map *map)
{
    unsigned int t,j;
    for (t=0;t<MAX_LEVELS;t++)
    {
        for (j=0;j<map->maxy;j++)
            free(map->tiles[t][j]);
        free(map->tiles[t]);
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
