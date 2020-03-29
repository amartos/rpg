#include "main.h"


int main(int argc, char *argv[])
{
    /* i is used for the "for loops".
     * time and prev_time are used for the framerate calculations.
     * The time_t type is not used to manage time here as SDL uses Uint32 types,
     * and casting from time_t (aka long) to uint is not advised. */
    int i = 0, time = 0, prev_time = 0;

    // SDL structures calls and init
    SDL_Window *window; SDL_Renderer *renderer;
    init_screen(&window, &renderer);
    SDL_Event event;

    /* Navigation loading
     * For now only one map is used to test the software.
     * TODO: load the last map the character were on when saved */
    Map map; init_map(&map, "test_map");
    Camera camera; init_camera(&camera); center_camera(&camera);

    /* Assets loading. This contains all infos for all the objects that will be
     * shown on screen. */
    Asset assets[0xFFFF]; init_asset_array(assets);
    load_assets_db(renderer, assets);

    /* Characters start position setting
     * For now the position is arbitrary and will change depending on the
     * previous save.
     * TODO: load characters previous position */
    for (i=FIRST_CHAR_ID;i<=LAST_CHAR_ID;i++)
    {
        assets[i].movement->position = int_to_coord(4,4);
        assets[i].movement->formation = SQUARE;
        deploy(&assets[i].movement->position, assets[i].movement, i-FIRST_CHAR_ID);
    }

    /* Cursors setting
     * Here we set the SDL_Rect w/h of the cursors to scale, and start with the
     * HOVER cursor type. */
    for (i=HOVER;i<=INVALID;i++)
    {
        assets[i].image->sdlrect.w = TILES_WIDTH(camera);
        assets[i].image->sdlrect.h = TILES_HEIGHT(camera);
    }
    Cursors mouse_type = HOVER;
    SDL_ShowCursor(SDL_DISABLE); // we use our own cursor, not the OS'

    // Some bools used to pause the game or end the program
    Bool done = FALSE, paused = FALSE;

    // Main loop
    while (!done)
    {
        while(SDL_PollEvent(&event))
            handle_event(event, assets, &camera, &mouse_type, map, &paused, &done);

        /* framerate limited events
         * this uses the time and prev_time ints */
        TIME
            for (i=FIRST_CHAR_ID;i<=LAST_CHAR_ID;i++)
                if (assets[i].movement->moving && !paused)
                {
                    move(assets[i].movement, map);
                    check_animation_frame(assets[i].animation, time);
                }

            render_screen(renderer, assets, mouse_type, camera, map, paused, FALSE);
            SDL_RenderPresent(renderer);
        ENDTIME
    }

    free_map(&map);
    free_assets_array(assets);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}
