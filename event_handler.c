#include "event_handler.h"


// events for special keys, eg. space, alt, ctrl, etc
static void push_special(SDL_Event const event, Bool *paused)
{
    switch(event.key.keysym.sym)
    {
        default:
            break;
        case SDLK_SPACE:
            *paused = !*paused;
            break;
    }
}

static void push_arrow(SDL_Event const event, Camera *camera)
{
    switch(event.key.keysym.sym)
    {
        default:
            break;
        case SDLK_UP:
            set_scroll(camera, -1, -1);
            break;
        case SDLK_DOWN:
            set_scroll(camera, +1, +1);
            break;
        case SDLK_LEFT:
            set_scroll(camera, -1, +1);
            break;
        case SDLK_RIGHT:
            set_scroll(camera, +1, -1);
            break;
    }
}

// events for Function keys F1...F12
static void push_fkey(SDL_Event const event, Asset assets[0xFFFF], Camera *camera)
{
    switch(event.key.keysym.sym)
    {
        default:
            break;
        case SDLK_F5:
            /* reset camera position to center and scale 1.
             * reset scale before, as coord depend on scale */
            reset_camera(camera);
            center_camera(camera);
            SCALE_CURSOR
            break;
    }
}

static void push_alpha(SDL_Event const event, Bool *quit, Asset assets[0xFFFF])
{
    switch(event.key.keysym.sym)
    {
        default:
            break;
        case SDLK_a:
            CHANGE_GROUP_FORMATION(LINE)
            break;
        case SDLK_z:
            CHANGE_GROUP_FORMATION(SQUARE)
            break;
        case SDLK_e:
            CHANGE_GROUP_FORMATION(TRIANGLE)
            break;
        case SDLK_r:
            CHANGE_GROUP_FORMATION(CIRCLE)
            break;
        case SDLK_q:
            *quit = TRUE;
            break;
    }
}

static void push_numeric(SDL_Event const event)
{
    switch(event.key.keysym.sym)
    {
        default:
            break;
    }
}

static void handle_keyboard(
        SDL_Event const event,
        Bool *paused,
        Bool *quit,
        Asset assets[0xFFFF],
        Camera *camera
        )
{
    switch (event.key.keysym.sym)
    {
        case SDLK_LCTRL:
        case SDLK_RCTRL:
        case SDLK_LSHIFT:
        case SDLK_RSHIFT:
        case SDLK_LALT:
        case SDLK_RALT: // alt-gr
        case SDLK_LGUI: // left super key
        case SDLK_RGUI: // right super key
        case SDLK_TAB:
        case SDLK_CAPSLOCK:
        case SDLK_BACKQUOTE:
        case SDLK_SPACE:
            push_special(event, paused);
            break;
        case SDLK_a:
        case SDLK_b:
        case SDLK_c:
        case SDLK_d:
        case SDLK_e:
        case SDLK_f:
        case SDLK_g:
        case SDLK_h:
        case SDLK_i:
        case SDLK_j:
        case SDLK_k:
        case SDLK_l:
        case SDLK_m:
        case SDLK_n:
        case SDLK_o:
        case SDLK_p:
        case SDLK_q:
        case SDLK_r:
        case SDLK_s:
        case SDLK_t:
        case SDLK_u:
        case SDLK_v:
        case SDLK_w:
        case SDLK_x:
        case SDLK_y:
        case SDLK_z:
            push_alpha(event, quit, assets);
            break;
        case SDLK_0:
        case SDLK_1:
        case SDLK_2:
        case SDLK_3:
        case SDLK_4:
        case SDLK_5:
        case SDLK_6:
        case SDLK_7:
        case SDLK_8:
        case SDLK_9:
            push_numeric(event);
            break;
        case SDLK_F1:
        case SDLK_F2:
        case SDLK_F3:
        case SDLK_F4:
        case SDLK_F5:
        case SDLK_F6:
        case SDLK_F7:
        case SDLK_F8:
        case SDLK_F9:
        case SDLK_F10:
        case SDLK_F11:
        case SDLK_F12:
            push_fkey(event, assets, camera);
            break;
        case SDLK_UP:
        case SDLK_DOWN:
        case SDLK_LEFT:
        case SDLK_RIGHT:
            push_arrow(event, camera);
            break;
    }
}

static void handle_mouse_wheel(SDL_Event const event, Camera *camera, Asset assets[0xFFFF])
{
    if (event.wheel.y > 0)
        set_zoom(camera, FALSE);
    else if (event.wheel.y < 0)
        set_zoom(camera, TRUE);
    SCALE_CURSOR
}

static void handle_mouse_motion(
        SDL_Event const event,
        Camera const camera,
        Asset assets[0xFFFF],
        Map const map,
        Cursors *mouse
        )
{
    // set cursor position for each cursor type
    for (unsigned int i=HOVER;i<=INVALID;i++)
        set_image_position(&assets[i].image->sdlrect, event.motion.x, event.motion.y);

    // determine which cursor to show: either HOVER or INVALID
    Coord position = screen_to_coord(event.motion.x, event.motion.y, camera);
    *mouse = HOVER;
    if (is_out_of_map(position, map))
        *mouse = INVALID;
}

static void handle_mouse_click(
        SDL_Event const event,
        Camera const camera,
        Asset assets[0xFFFF],
        Map const map
        )
{
    /* We use two sets of coord, click will be used as a save and position will
     * be the one modified. */
    Coord click = screen_to_coord(event.button.x, event.button.y, camera);
    Coord position; init_coord(&position);

    for (unsigned int i=FIRST_CHAR_ID;i<=LAST_CHAR_ID;i++)
    {
        position = click;
        if (is_pos_legal(position, assets[i].movement->position, map))
        {
            stop_movement(assets[i].movement);
            deploy(&position, assets[i].movement, i-FIRST_CHAR_ID);
            assets[i].movement->path[0] = position;
            switch(event.button.button)
            {
                default:
                    stop_movement(assets[i].movement);
                    break;
                case SDL_BUTTON_LEFT:
                    fire_movement(assets[i].movement, PATH);
                    break;
                case SDL_BUTTON_RIGHT:
                    fire_movement(assets[i].movement, TELEPORT);
                    break;
            }
        }
    }
}

static void handle_mouse(SDL_Event const event, Camera *camera, Asset assets[0xFFFF], Map const map, Cursors *mouse)
{
    switch(event.type)
    {
        case SDL_MOUSEMOTION:
            handle_mouse_motion(event, *camera, assets, map, mouse);
            break;
        case SDL_MOUSEBUTTONDOWN:
            handle_mouse_click(event, *camera, assets, map);
            break;
        case SDL_MOUSEWHEEL:
            handle_mouse_wheel(event, camera, assets);
            break;
    }
}

void handle_event(
        SDL_Event const event,
        Asset assets[0xFFFF],
        Camera *camera,
        Cursors *mouse,
        Map const map,
        Bool *paused,
        Bool *quit
        )
{
    switch(event.type)
    {
        default:
            break;
        case SDL_MOUSEMOTION:
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEWHEEL:
            handle_mouse(event, camera, assets, map, mouse);
            break;
        case SDL_KEYDOWN:
            handle_keyboard(event, paused, quit, assets, camera);
            break;
        case SDL_QUIT:
            *quit = TRUE;
            break;
    }

}
