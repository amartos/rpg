#include "pathfinding_test.h"


SDL_Texture* make_colored_rect(SDL_Renderer *renderer, int const R, int const G, int const B)
{
    SDL_Surface *surface = SDL_CreateRGBSurface(0, BASE_TEST_TILES_WIDTH, BASE_TEST_TILES_HEIGHT, SCREEN_BPP, 0, 0, 0, 0);
    SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, R, G, B));
    surface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA8888, 0);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

void render_map(
        SDL_Renderer *renderer,
        Coord const start_coord,
        Camera const camera,
        SDL_Texture* const textures[],
        Map const map,
        Coord const path[MAX_PATH_NODES],
        unsigned int nodes
        )
{
    SDL_Rect start; init_sdl_rect(&start);
    set_image_position(
            &start,
            (start_coord.x+camera.scroll.x)*TEST_TILES_WIDTH(camera),
            (start_coord.y+camera.scroll.y)*TEST_TILES_HEIGHT(camera)
            );
    set_image_size(&start, TEST_TILES_WIDTH(camera), TEST_TILES_HEIGHT(camera));

    // Rendering
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF); // RGBA
    SDL_RenderDrawRect(renderer, NULL);

    SDL_RenderCopy(renderer, textures[0], NULL, &start);

    SDL_Rect position; init_sdl_rect(&position);
    set_image_size(&position, TEST_TILES_WIDTH(camera), TEST_TILES_HEIGHT(camera));
    for (unsigned int y=0;y<map.max.y;y++)
        for (unsigned int x=0;x<map.max.x;x++)
        {
            set_image_position(
                    &position,
                    (x+camera.scroll.x) * TEST_TILES_WIDTH(camera),
                    (y+camera.scroll.y) * TEST_TILES_HEIGHT(camera)
                    );
            if (map.collisions[y][x])
                SDL_RenderCopy(renderer, textures[1], NULL, &position);
            else if (map.cost[y][x])
            {
                switch(map.cost[y][x])
                {
                    default:
                        break;
                    case 10:
                        SDL_RenderCopy(renderer, textures[3], NULL, &position);
                        break;
                    case 5:
                        SDL_RenderCopy(renderer, textures[2], NULL, &position);
                        break;
                }
            }
        }

    if (nodes)
    {
        for (unsigned int i=0;i<nodes;i++)
        {
            set_image_position(
                    &position,
                    (path[i].x+camera.scroll.x) * TEST_TILES_WIDTH(camera),
                    (path[i].y+camera.scroll.y) * TEST_TILES_HEIGHT(camera)
                    );
            SDL_RenderCopy(renderer, textures[4], NULL, &position);
        }
        nodes = 0;
    }
}


int main(int argc, char *argv[])
{
    unsigned int nodes = 0, time = 0, prev_time = 0;
    Bool done = FALSE;

    // SDL vars init
    SDL_Window *window; SDL_Renderer *renderer;
    init_screen(&window, &renderer);

    SDL_Texture* textures[5];
    textures[0] = make_colored_rect(renderer, 0, 255, 0); // start
    textures[1] = make_colored_rect(renderer, 0, 0, 0); // black
    textures[2] = make_colored_rect(renderer, 205, 205, 205); // grey cost 5
    textures[3] = make_colored_rect(renderer, 155, 155, 155); // grey cost 10
    textures[4] = make_colored_rect(renderer, 0, 128, 0); // green

    Map map; init_map(&map, "pathfinding_test");

    Camera camera; init_camera(&camera);

    Coord path[MAX_PATH_NODES];
    for (int i=0;i<MAX_PATH_NODES;i++)
        init_coord(&path[i]);

    Coord click; init_coord(&click);
    Coord start = int_to_coord(STARTX, STARTY);

    // main loop
    SDL_Event event;
    while (!done)
    {
        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
                default:
                    break;
                case SDL_QUIT:
                    done = TRUE;
                    break;
                case SDL_MOUSEWHEEL:
                    if (event.wheel.y > 0)
                        set_zoom(&camera, FALSE);
                    else if (event.wheel.y < 0)
                        set_zoom(&camera, TRUE);
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    // This is done because the screen_to_coord conversion
                    // function uses other TILES dimensions
                    click = int_to_coord(
                            (event.button.x / TEST_TILES_WIDTH(camera)) - camera.scroll.x,
                            (event.button.y / TEST_TILES_HEIGHT(camera)) - camera.scroll.y
                            );
                    switch(event.button.button)
                    {
                        default:
                            break;
                        case SDL_BUTTON_LEFT:
                            if (is_pos_legal(click, start, map))
                                nodes = find_path(path, start, click, map);
                            break;
                        case SDL_BUTTON_RIGHT:
                            if (!is_colliding(click, map) && !is_out_of_map(click, map))
                                start = click;
                            break;
                    }
                    break;
                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym)
                    {
                        case SDLK_q:
                            done = TRUE;
                            break;
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
        }
        /* framerate limited events
         * this uses the time and prev_time ints */
        TIME
            render_map(renderer, start, camera, textures, map, path, nodes);
            SDL_RenderPresent(renderer);
        ENDTIME
    }

    for (unsigned int i = 0;i<5;i++)
        SDL_DestroyTexture(textures[i]);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    free_map(&map);

    return EXIT_SUCCESS;
}
