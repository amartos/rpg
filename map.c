#include "map.h"

void init_map(Map *map, const char map_path[])
{
    FILE *map_file = NULL;
    /* A tile converter file will probably be used in the future
     * FILE *converter_file = NULL; */
    int i = 0, j = 0, line_count = 0, column_count = 0, len = 0;
    unsigned int t = 0, n_types = WEATHER + 1; // weather always last
    char line[MAX_SIZE_LINE] = {0};

    TRY
    {
        map_file = fopen(map_path, "r");
        if (map_file == NULL)
            THROW(MAP_FILE_LOADING_FAILURE);

        /* converter_file = fopen("assets/maps/converter", "r"); */
        /* if (converter_file == NULL) */
        /*     THROW(CONVERTER_FILE_LOADING_FAILURE); */
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

    /* size of map
     * len of line should always be a multiple of 5
     * (4 hex digit + space/en of line) */
    while (fgets(line, MAX_SIZE_LINE, map_file) != NULL)
    {
        len = strlen(line)/5;
        if (len > column_count)
            column_count = len;
        if (line[0] != '#' && line[0] != '\n')
            line_count++;
    }

    line_count = line_count/(n_types-1); // BG and FG in same map
    map->x_tiles = column_count, map->y_tiles = line_count;
    map->total_tiles = map->x_tiles + map->y_tiles;
    map->w = TILES_WIDTH * column_count, map->h = TILES_HEIGHT * line_count;
    map->xscroll = map->yscroll = 0;

    // TODO: malloc needs to be better checked
    TRY
    {
        map->schematics = malloc(sizeof(unsigned int *) * n_types);
        if (map->schematics == NULL)
            THROW(MAP_MALLOC_FAILURE);

        for (t=BACKGROUND;t<n_types;t++)
        {
            map->schematics[t] = malloc(sizeof(unsigned int *) * column_count);
            if (map->schematics[t] == NULL)
                THROW(MAP_MALLOC_FAILURE);

            for(i=0;i<column_count;i++)
            {
                map->schematics[t][i] = malloc(sizeof(unsigned int) * line_count);
                if (map->schematics[t][i] == NULL)
                    THROW(MAP_MALLOC_FAILURE);

                for (j=0;j<line_count;j++)
                    map->schematics[t][i][j] = 0;
            }
        }
    }
    CATCH(MAP_MALLOC_FAILURE)
    {
        logger(MAP_MALLOC_FAILURE, "oups");
        exit(EXIT_FAILURE);
    }
    ETRY;

    rewind(map_file);

    /* This is a nasty piece of code as it supposes that the map files are all
     * constructed EXACTLY the same way, and that all the map lines are of the
     * same length (a multiple of 4 digits hex numbers + 1 space). It is too 
     * much constraint, but it's a start. If RLE compression is implemented, 
     * this code will definitely need to change */
    char *data = NULL;
    int offset = 0;
    int map_type = 0;
    while (fgets(line, MAX_SIZE_LINE, map_file) != NULL)
    {
        if (!strcmp(line, "# tiles\n"))
        {
            map_type = BACKGROUND;
            j = 0;
            offset = 5;
        }
        else if (!strcmp(line, "# collisions\n"))
        {
            map_type = COLLISIONS;
            j = 0;
            offset = 2;
        }
        else if (!strcmp(line, "# weather\n"))
        {
            map_type = WEATHER;
            j = 0;
            offset = 5;
        }
        else if (!strcmp(line, "# movement cost\n"))
        {
            map_type = COST;
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
                case BACKGROUND:
                    for (i=0;i<column_count;i++)
                    {
                        sscanf(data, "%X ", &map->schematics[BACKGROUND][i][j]);
                        if (map->schematics[BACKGROUND][i][j] == 0x0101)
                        {
                            map->schematics[FOREGROUND][i][j] = map->schematics[BACKGROUND][i][j];
                            map->schematics[BACKGROUND][i][j] = 0;
                        }
                        data += offset;
                    }
                    break;
                case COLLISIONS:
                    for (i=0;i<column_count;i++)
                    {
                        sscanf(data, "%d ", &map->schematics[COLLISIONS][i][j]);
                        data += offset;
                    }
                    break;
                case WEATHER:
                    for (i=0;i<column_count;i++)
                    {
                        sscanf(data, "%X ", &map->schematics[WEATHER][i][j]);
                        data += offset;
                    }
                    break;
                case COST:
                    for (i=0;i<column_count;i++)
                    {
                        sscanf(data, "%X ", &map->schematics[COST][i][j]);
                        data += offset;
                    }
                    break;
            }
            j++;
        }
    }

    fclose(map_file);
    /* fclose(converter_file); */
}

void free_map(Map *map)
{
    unsigned int t,i, n_types = WEATHER + 1;
    for (t=BACKGROUND;t<n_types;t++)
    {
        for (i=0;i<map->x_tiles;i++)
            free(map->schematics[t][i]);
        free(map->schematics[t]);
    }
}
