#include "../errors.h"
#include "../try_throw_catch.h"
#include "../macros.h"

#include "../coord.h"
#include "../pathfinding.h"

#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <math.h>


int main(int argc, char *argv[])
{
    unsigned int i = 0, j = 0, nodes = 0, ncurrent = 0, score = 0, max = 0;
    Bool done = FALSE;

    // SDL vars init
    SDL_Surface *screen = NULL;
    TRY
    {
        if (SDL_Init(SDL_INIT_VIDEO))
            THROW(SCREEN_INIT_FAILURE);
        screen = SDL_SetVideoMode(
                640, 640, 32, 
                SDL_RESIZABLE | SDL_HWSURFACE | SDL_DOUBLEBUF
                );
        if (screen == NULL)
            THROW(VIDEO_MODE_LOAD_FAILURE);
    }
    CATCH(SCREEN_INIT_FAILURE)
    {
        logger(SCREEN_INIT_FAILURE, SDL_GetError());
        exit(EXIT_FAILURE);
    }
    CATCH(VIDEO_MODE_LOAD_FAILURE)
    {
        logger(SCREEN_INIT_FAILURE, SDL_GetError());
        exit(EXIT_FAILURE);
    }
    ETRY;

    SDL_Rect infos;
    infos.x = 0; infos.y = 0;
    infos.w = 32; infos.h = 32;

    Coord max_coord; init_coord(&max_coord);
    max_coord.x = 20; max_coord.y = 20;

    Coord start; init_coord(&start);
    start.x = 5*32; start.y = 5*32;
    Coord start_units; init_coord(&start_units);
    start_units.x = 5; start_units.y = 5;

    unsigned int **collision = NULL; 
    unsigned int **cost = NULL; 
    unsigned int coll1[20][20] = 
    {
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1},
        {1,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1},
        {1,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1},
        {1,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1},
        {1,0,1,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,1},
        {1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,1,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,1},
        {1,0,1,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,1},
        {1,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1},
        {1,0,1,0,0,0,0,0,1,1,0,0,0,1,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,1,1,1,0,0,0,1,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,1,0,1,0,0,0,1,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,1,0,1,0,0,0,1,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
    };

    unsigned int cost1[20][20] = 
    {
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,10,10,10,10,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,10,10,10,10,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,10,10,10,10,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,10,10,10,10,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,10,10,10,10,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,5,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,5,0,0},
        {0,0,0,5,5,0,0,0,0,0,0,0,0,0,0,0,5,5,0,0},
        {0,0,0,5,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,5,5,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,5,5,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
    };

    collision = malloc(sizeof(unsigned int*) * max_coord.x);
    cost = malloc(sizeof(unsigned int*) * max_coord.x);
    if (collision == NULL || cost == NULL)
        exit(EXIT_FAILURE);
    for(i=0;i<max_coord.x;i++)
    {
        collision[i] = malloc(sizeof(unsigned int) * max_coord.x);
        cost[i] = malloc(sizeof(unsigned int) * max_coord.x);
        if (collision[i] == NULL || cost[i] == NULL)
            exit(EXIT_FAILURE);

        for (j=0;j<max_coord.y;j++)
        {
            collision[i][j] = coll1[j][i];
            cost[i][j] = cost1[j][i];
        }
    }

    unsigned int scores[MAX_PATH_NODES];
    for (i=0;i<MAX_PATH_NODES;i++)
        scores[i] = 0;
    Coord path[MAX_PATH_NODES];
    for (i=0;i<MAX_PATH_NODES;i++)
        init_coord(&path[i]);

    // main loop
    Coord click; init_coord(&click);
    SDL_Event event;
    while (!done)
    {
        SDL_WaitEvent(&event);
        switch(event.type)
        {
            case SDL_QUIT:
                done = TRUE;
                break;
            case SDL_MOUSEBUTTONDOWN:
                switch(event.button.button)
                {
                    case SDL_BUTTON_LEFT:
                        click.x = event.button.x / TILES_WIDTH;
                        click.y = event.button.y / TILES_HEIGHT;
                        nodes = find_path(
                                path,
                                start_units, click,
                                max_coord.x, max_coord.y,
                                collision, cost,
                                scores
                                );
                        break;
                    case SDL_BUTTON_RIGHT:
                        break;
                }
                break;
        }
       
        SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0xff, 0xff, 0xff));
        infos.x = start.x; infos.y = start.y;
        SDL_FillRect(screen, &infos, SDL_MapRGB(screen->format, 0, 0xff, 0));
        for (i=0;i<max_coord.x;i++)
            for (j=0;j<max_coord.y;j++)
            {
                infos.x = i * 32; infos.y = j * 32;
                if (collision[i][j])
                    SDL_FillRect(screen, &infos, SDL_MapRGB(screen->format, 0, 0, 0));
                else if (cost[i][j])
                    SDL_FillRect(screen, &infos, SDL_MapRGB(screen->format, 255-10*cost[i][j], 255-10*cost[i][j], 255-10*cost[i][j]));
            }
        if (nodes)
        {
            for (i=0;i<nodes;i++)
                if (max < scores[i])
                    max = scores[i];
            for (i=0;i<nodes;i++)
            {
                infos.x = path[i].x * 32; infos.y = path[i].y * 32;
                ncurrent = max_coord.x * path[i].y + path[i].x;
                score = 255 * scores[i] / max;
                SDL_FillRect(screen, &infos, SDL_MapRGB(screen->format, score, 0, 255 - score));
            }
            nodes = 0;
            for (i=0;i<MAX_PATH_NODES;i++)
                scores[i] = 0;
        }

        TRY
        {
            if (SDL_Flip(screen))
                THROW(FLIP_SCREEN_FAILURE);
        }
        CATCH(FLIP_SCREEN_FAILURE)
        {
            logger(FLIP_SCREEN_FAILURE, SDL_GetError());
            exit(EXIT_FAILURE);
        }
        ETRY;
    }
    SDL_Quit();
    for (i=0;i<max_coord.x;i++)
    {
        free(collision[i]);
        free(cost[i]);
    }
    return 0;
}
