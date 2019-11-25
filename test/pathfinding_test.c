#include "../errors.h"
#include "../try_throw_catch.h"

#include "../coord.h"
#include "../map.h"
#include "../database.h"
#include "../pathfinding.h"

#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <math.h>

#define STARTX 5
#define STARTY 5
#define TEST_TILES_WIDTH 16
#define TEST_TILES_HEIGHT 16

int main(int argc, char *argv[])
{
    unsigned int i = 0, j = 0, nodes = 0, ncurrent = 0, score = 0, max = 0;
    Bool done = FALSE;

    // SDL vars init
    SDL_Window *window; SDL_Renderer *renderer;
    TRY
    {
        if (SDL_Init(SDL_INIT_VIDEO))
            THROW(VIDEO_INIT_FAILURE);
        if (
                SDL_CreateWindowAndRenderer(
                0, 0, // to replace with FULL_SCREEN_DESKTOP
                SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED,
                &window, &renderer
                )
           )
            THROW(WINDOW_INIT_FAILURE);
    }
    CATCH_HANDLE(SDL_INIT_VIDEO, NULL)
    CATCH_HANDLE(WINDOW_INIT_FAILURE, NULL)
    ETRY;

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear"); // softer resizes 
    SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);

    SDL_Rect infos;
    infos.x = 0; infos.y = 0;
    infos.w = TEST_TILES_WIDTH; infos.h = TEST_TILES_HEIGHT;

    SDL_Surface *black_rect_surface = SDL_CreateRGBSurface(0, TEST_TILES_WIDTH, TEST_TILES_HEIGHT, SCREEN_BPP, 0, 0, 0, 0);
    SDL_FillRect(black_rect_surface, NULL, SDL_MapRGB(black_rect_surface->format, 0, 0, 0));
    black_rect_surface = SDL_ConvertSurfaceFormat(black_rect_surface, SDL_PIXELFORMAT_RGBA8888, 0);
    SDL_Texture *black_rect = SDL_CreateTextureFromSurface(renderer, black_rect_surface);
    SDL_FreeSurface(black_rect_surface);

    SDL_Surface *grey10_rect_surface = SDL_CreateRGBSurface(0, TEST_TILES_WIDTH, TEST_TILES_HEIGHT, SCREEN_BPP, 0, 0, 0, 0);
    SDL_FillRect(grey10_rect_surface, NULL, SDL_MapRGB(grey10_rect_surface->format, 155, 155, 155));
    grey10_rect_surface = SDL_ConvertSurfaceFormat(grey10_rect_surface, SDL_PIXELFORMAT_RGBA8888, 0);
    SDL_Texture *grey10_rect = SDL_CreateTextureFromSurface(renderer, grey10_rect_surface);
    SDL_FreeSurface(grey10_rect_surface);

    SDL_Surface *grey5_rect_surface = SDL_CreateRGBSurface(0, TEST_TILES_WIDTH, TEST_TILES_HEIGHT, SCREEN_BPP, 0, 0, 0, 0);
    SDL_FillRect(grey5_rect_surface, NULL, SDL_MapRGB(grey5_rect_surface->format, 205, 205, 205));
    grey5_rect_surface = SDL_ConvertSurfaceFormat(grey5_rect_surface, SDL_PIXELFORMAT_RGBA8888, 0);
    SDL_Texture *grey5_rect = SDL_CreateTextureFromSurface(renderer, grey5_rect_surface);
    SDL_FreeSurface(grey5_rect_surface);

    SDL_Surface *green_rect_surface = SDL_CreateRGBSurface(0, TEST_TILES_WIDTH/2, TEST_TILES_HEIGHT/2, SCREEN_BPP, 0, 0, 0, 0);
    SDL_FillRect(green_rect_surface, NULL, SDL_MapRGB(green_rect_surface->format, 0, 128, 0));
    green_rect_surface = SDL_ConvertSurfaceFormat(green_rect_surface, SDL_PIXELFORMAT_RGBA8888, 0);
    SDL_Texture *green_rect = SDL_CreateTextureFromSurface(renderer, green_rect_surface);
    SDL_FreeSurface(green_rect_surface);

    SDL_Surface *start_rect_surface = SDL_CreateRGBSurface(0, TEST_TILES_WIDTH, TEST_TILES_HEIGHT, SCREEN_BPP, 0, 0, 0, 0);
    SDL_FillRect(start_rect_surface, NULL, SDL_MapRGB(start_rect_surface->format, 0, 255, 0));
    start_rect_surface = SDL_ConvertSurfaceFormat(start_rect_surface, SDL_PIXELFORMAT_RGBA8888, 0);
    SDL_Texture *start_rect = SDL_CreateTextureFromSurface(renderer, start_rect_surface);
    SDL_FreeSurface(start_rect_surface);

    SDL_Rect start;
    start.w = TEST_TILES_WIDTH; start.h = TEST_TILES_HEIGHT;
    Coord start_units; init_coord(&start_units);
    start_units.x = STARTX; start_units.y = STARTY;

    Coord coord; init_coord(&coord);
    Coord max_coord; init_coord(&max_coord);

    Map map; init_map(&map, "PFtest");
    max_coord.x = map.maxx; max_coord.y = map.maxy;

    Camera camera; init_camera(&camera);

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
                        click.x = (event.button.x / TEST_TILES_WIDTH) - camera.scroll.x;
                        click.y = (event.button.y / TEST_TILES_HEIGHT) - camera.scroll.y;
                        if (
                                is_pos_legal(click, start_units, max_coord, map.collisions) &&
                                !is_colliding(click, map.collisions, max_coord)
                           )
                            nodes = find_path(
                                    path,
                                    start_units, click,
                                    max_coord,
                                    map.collisions, map.cost
                                    );
                        break;
                    case SDL_BUTTON_RIGHT:
                        break;
                }
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                    case SDLK_UP:
                        camera.scroll.y += 1;
                        break;
                    case SDLK_DOWN:
                        camera.scroll.y -= 1;
                        break;
                    case SDLK_LEFT:
                        camera.scroll.x += 1;
                        break;
                    case SDLK_RIGHT:
                        camera.scroll.x -= 1;
                        break;
                }
                break;
        }

        // Rendering
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF); // RGBA
        SDL_RenderDrawRect(renderer, NULL);

        start.x = (STARTX+camera.scroll.x)*TEST_TILES_WIDTH;
        start.y = (STARTY+camera.scroll.y)*TEST_TILES_HEIGHT;
        SDL_RenderCopy(renderer, start_rect, NULL, &start);
        for (j=0;j<max_coord.y;j++)
            for (i=0;i<max_coord.x;i++)
            {
                infos.x = (i+camera.scroll.x) * TEST_TILES_WIDTH;
                infos.y = (j+camera.scroll.y) * TEST_TILES_HEIGHT;
                if (map.collisions[j][i])
                    SDL_RenderCopy(renderer, black_rect, NULL, &infos);
                else if (map.cost[j][i])
                {
                    if (map.cost[j][i] == 10)
                        SDL_RenderCopy(renderer, grey10_rect, NULL, &infos);
                    else if (map.cost[j][i] == 5)
                        SDL_RenderCopy(renderer, grey5_rect, NULL, &infos);
                }
            }

        if (nodes)
        {
            for (i=0;i<nodes;i++)
            {
                infos.x = (path[i].x+camera.scroll.x) * TEST_TILES_WIDTH;
                infos.y = (path[i].y+camera.scroll.y) * TEST_TILES_HEIGHT;
                SDL_RenderCopy(renderer, green_rect, NULL, &infos);
            }
            nodes = 0;
        }

        SDL_RenderPresent(renderer);
    }
    SDL_DestroyTexture(black_rect);
    SDL_DestroyTexture(grey10_rect);
    SDL_DestroyTexture(grey5_rect);
    SDL_DestroyTexture(green_rect);
    SDL_DestroyTexture(start_rect);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    TTF_Quit();
    free_map(&map);
    return 0;
}
