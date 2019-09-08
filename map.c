#include "map.h"


void init_map(Map *map, char const map_path[])
{
    FILE *map_file = NULL;// FILE *converter_file = NULL;
    unsigned int i = 0, j = 0, line_count = 0, column_count = 0, len = 0;
    unsigned int t = 0, n_types = WEATHER + 1; // weather always last
    char line[MAX_SIZE_LINE] = {0};

    TRY
    {
        map_file = fopen(map_path, "r");
        if (map_file == NULL)
            THROW(MAP_FILE_LOADING_FAILURE);

        // converter_file = fopen("assets/maps/converter", "r");
        // if (converter_file == NULL)
        //     THROW(CONVERTER_FILE_LOADING_FAILURE);
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
    data = line; sscanf(data, "x: %d", &map->x_tiles);
    fgets(line, MAX_SIZE_LINE, map_file);
    data = line; sscanf(data, "y: %d", &map->y_tiles);

    map->total_tiles = map->x_tiles * map->y_tiles;
    map->w = TILES_WIDTH * map->x_tiles, map->h = TILES_HEIGHT * map->y_tiles;
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

    // This is a nasty piece of code as it supposes that the map files are all
    // constructed EXACTLY the same way, and that all the map lines are of the
    // same length (a multiple of 4 digits hex numbers + 1 space). It is too 
    // much constraint, but it's a start. If RLE compression is implemented, 
    // this code will definitely need to change
    char *data = NULL;
    unsigned int offset = 0;
    MapType map_type = 0;
    while (fgets(line, MAX_SIZE_LINE, map_file) != NULL)
    {
        if (!strcmp(line, "# background\n"))
        {
            map_type = BACKGROUND;
            j = 0;
            offset = 5;
        }
        else if (!strcmp(line, "# foreground\n"))
        {
            map_type = FOREGROUND;
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
                case FOREGROUND:
                    for (i=0;i<column_count;i++)
                    {
                        sscanf(data, "%X ", &map->schematics[FOREGROUND][i][j]);
                        data += offset;
                    }
                    break;
                case BACKGROUND:
                    for (i=0;i<column_count;i++)
                    {
                        sscanf(data, "%X ", &map->schematics[BACKGROUND][i][j]);
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
    // fclose(converter_file);
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
    free(map->schematics);
}

void init_empty_map(Map *map, unsigned int const x_tiles, unsigned int const y_tiles)
{
    unsigned int i = 0, j = 0, t = 0;

    map->x_tiles = x_tiles, map->y_tiles = y_tiles;
    map->total_tiles = map->x_tiles * map->y_tiles;
    map->w = TILES_WIDTH * x_tiles; map->h = TILES_HEIGHT * y_tiles;
    map->xscroll = map->yscroll = 0;

    // TODO: malloc needs to be better checked
    TRY
    {
        map->schematics = malloc(sizeof(unsigned int *) * (WEATHER+1));
        if (map->schematics == NULL)
            THROW(MAP_MALLOC_FAILURE);

        for (t=BACKGROUND;t<=WEATHER;t++)
        {
            map->schematics[t] = malloc(sizeof(unsigned int *) * x_tiles);
            if (map->schematics[t] == NULL)
                THROW(MAP_MALLOC_FAILURE);

            for(i=0;i<x_tiles;i++)
            {
                map->schematics[t][i] = malloc(sizeof(unsigned int) * y_tiles);
                if (map->schematics[t][i] == NULL)
                    THROW(MAP_MALLOC_FAILURE);

                for (j=0;j<y_tiles;j++)
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
}
