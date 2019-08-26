#include "map.h"

void init_map(Map *map, const char map_path[])
{
    FILE *map_file = NULL;
    /* A tile converter file will probably be used in the future
     * FILE *converter_file = NULL; */
    int i = 0, j = 0, line_count = 0, column_count = 0, len = 0;
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

    line_count = line_count/3;
    map->infos.x = column_count;
    map->infos.y = line_count;
    map->infos.w = TILES_WIDTH * column_count;
    map->infos.h = TILES_HEIGHT * line_count;

    // malloc needs to be better checked
    TRY
    {

        map->tiles = malloc(sizeof(unsigned int *) * column_count);
        map->collisions = malloc(sizeof(unsigned int *) * column_count);
        map->weather = malloc(sizeof(unsigned int *) * column_count);

        if (map->tiles == NULL || map->collisions == NULL || map->weather == NULL)
            THROW(MAP_MALLOC_FAILURE);

        for(i=0;i<column_count; i++)
        {
            map->tiles[i] = malloc(sizeof(unsigned int) * line_count);
            map->collisions[i] = malloc(sizeof(unsigned int) * line_count);
            map->weather[i] = malloc(sizeof(unsigned int) * line_count);
            if (map->tiles[i] == NULL || map->collisions[i] == NULL || map->weather[i] == NULL)
                THROW(MAP_MALLOC_FAILURE);
        }
      
        for (i=0;i<column_count;i++) 
            for (j=0;j<line_count;j++)
            {
                map->tiles[i][j] = 0;
                map->collisions[i][j] = 0;
                map->weather[i][j] = 0;
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
            map_type = 1;
            j = 0;
            offset = 5;
        }
        else if (!strcmp(line, "# collisions\n"))
        {
            map_type = 2;
            j = 0;
            offset = 2;
        }
        else if (!strcmp(line, "# weather\n"))
        {
            map_type = 3;
            j = 0;
            offset = 5;
        }
        else if (!strcmp(line, "\n"))
            continue;
        else
        {
            data = line;
            switch (map_type)
            {
                case 1:
                    for (i=0;i<column_count;i++)
                    {
                        sscanf(data, "%X ", &map->tiles[i][j]);
                        data += offset;
                    }
                    break;
                case 2:
                    for (i=0;i<column_count;i++)
                    {
                        sscanf(data, "%d ", &map->collisions[i][j]);
                        data += offset;
                    }
                    break;
                case 3:
                    for (i=0;i<column_count;i++)
                    {
                        sscanf(data, "%X ", &map->weather[i][j]);
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
    int j;
    for (j=0;j<map->infos.y;j++)
    {
        free(map->tiles[j]);
        free(map->collisions[j]);
        free(map->weather[j]);
    }
    free(map->tiles);
    free(map->collisions);
    free(map->weather);
}
