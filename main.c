#include "main.h"


int main(int argc, char *argv[])
{
    unsigned int i = 0, time = 0, prev_time = 0;

    // SDL vars init
    SDL_Window *window; SDL_Renderer *renderer;
    init_screen(&window, &renderer);

    Asset assets[0xFFFF]; init_asset_array(assets); load_assets_db(renderer, assets);
    SDL_Texture *pause_layer = make_colored_rect(renderer, TILES_WIDTH, TILES_HEIGHT, 0, 0, 0, 50);

    SDL_Rect sprites_infos; init_sdl_rect(&sprites_infos);
    sprites_infos.w = SPRITES_WIDTH; sprites_infos.h = SPRITES_HEIGHT;

    SDL_Rect mouse_hover_rect; init_sdl_rect(&mouse_hover_rect);
    mouse_hover_rect.w = TILES_WIDTH; mouse_hover_rect.h = TILES_HEIGHT;

    SDL_Rect tiles_infos; init_sdl_rect(&tiles_infos);

    SDL_Event event;

    // custom structs init
    Bool done = FALSE, paused = FALSE;

    Cursors mouse_type = HOVER;

    Coord max_coord; init_coord(&max_coord);
    Coord position; init_coord(&position);
    Coord scroll; init_coord(&scroll);
    scroll.x = (SCREEN_WIDTH/2) / TILES_WIDTH;
    scroll.y = (SCREEN_HEIGHT/2) / TILES_HEIGHT;

    // load maps
    Map test_map; init_map(&test_map);
    max_coord.x = test_map.maxx; max_coord.y = test_map.maxy;

    // load characters
    Asset all_characters[MAX_CHARACTERS];
    position.x = 4.0; position.y = 4.0;
    for (i=0;i<MAX_CHARACTERS;i++)
    {
        all_characters[i] = assets[0x100+i];
        all_characters[i].movement->position = position;
        all_characters[i].movement->formation = SQUARE;
        deploy(
                &all_characters[i].movement->position,
                all_characters[i].movement->direction,
                all_characters[i].movement->formation,
                i
                );
    }

    // main loop
    while (!done)
    {
        // framerate limited events
        // this contains drawing on screen and characters movements
        time = SDL_GetTicks();
        if (time - prev_time > FRAMERATE)
        {
            prev_time = time;
            for (i=0;i<MAX_CHARACTERS;i++)
                if (all_characters[i].movement->moving && !paused)
                {
                    move(
                            all_characters[i].movement,
                            max_coord,
                            test_map.collisions,
                            test_map.cost
                            );
                    check_animobj_frame(all_characters[i].animation, time);
                }

            render_screen(renderer,
                    assets, pause_layer,
                    mouse_type, mouse_hover_rect,
                    scroll,
                    test_map,
                    paused
                    );
        }
        else // do not overuse CPU
            SDL_Delay(FRAMERATE - (time - prev_time));

        // events handling
        while(SDL_PollEvent(&event))
            switch(event.type)
            {
                case SDL_QUIT:
                    done = TRUE;
                    break;
                case SDL_MOUSEMOTION:
                    mouse_type = handle_mouse_motion(event, &mouse_hover_rect, max_coord);
                    break;
                case SDL_MOUSEBUTTONUP:
                case SDL_MOUSEBUTTONDOWN:
                    handle_mouse_click(event, scroll, all_characters, MAX_CHARACTERS, test_map);
                    break;
                case SDL_KEYDOWN:
                    handle_keyboard(event, &paused, all_characters, &scroll);
                    break;
            }

        // Render !
        SDL_RenderPresent(renderer);
    }

    // free all freeables
    free_map(&test_map);
    free_assets_array(assets);
    SDL_DestroyTexture(pause_layer);
    SDL_Quit();

    return EXIT_SUCCESS;
}
