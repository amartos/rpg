#include "event_handler.h"


void handle_keyboard(
        SDL_Event event, Bool *paused,
        Asset characters[MAX_CHARACTERS],
        Coord *scroll
        )
{
    unsigned int i;
    switch (event.key.keysym.sym)
    {
        case SDLK_SPACE:
            *paused = !*paused;
            break;
        case SDLK_a:
            for (i=0;i<MAX_CHARACTERS;i++)
                change_formation(characters[i].movement, LINE);
            break;
        case SDLK_z:
            for (i=0;i<MAX_CHARACTERS;i++)
                change_formation(characters[i].movement, SQUARE);
            break;
        case SDLK_e:
            for (i=0;i<MAX_CHARACTERS;i++)
                change_formation(characters[i].movement, TRIANGLE);
            break;
        case SDLK_r:
            for (i=0;i<MAX_CHARACTERS;i++)
                change_formation(characters[i].movement, CIRCLE);
            break;
        case SDLK_F5:
            scroll->x = (SCREEN_WIDTH/2)/TILES_WIDTH;
            scroll->y = (SCREEN_HEIGHT/2)/TILES_HEIGHT;
            break;
        case SDLK_UP:
            scroll->x -= 1;
            scroll->y -= 1;
            break;
        case SDLK_DOWN:
            scroll->x += 1;
            scroll->y += 1;
            break;
        case SDLK_LEFT:
            scroll->x -= 1;
            scroll->y += 1;
            break;
        case SDLK_RIGHT:
            scroll->x += 1;
            scroll->y -= 1;
            break;
    }
}

Cursors handle_mouse_motion(
        SDL_Event const event,
        Coord const scroll,
        SDL_Rect *mouse_hover_rect,
        Coord const max_coord
        )
{
    mouse_hover_rect->x = event.motion.x;
    mouse_hover_rect->y = event.motion.y;

    Coord position = event_to_coord(event.motion.x, event.motion.y, scroll);
    if (!is_out_of_map(position, max_coord))
        return HOVER;
    else
        return INVALID;
}

void handle_mouse_click(
        SDL_Event event,
        Coord const scroll,
        Asset characters[MAX_CHARACTERS],
        unsigned int max_objects,
        Map const map
        )
{
    unsigned int i;

    Coord max_coord = int_to_coord(map.maxx, map.maxy);

    Coord position = event_to_coord(event.button.x, event.button.y, scroll);

    Coord coord; init_coord(&coord);

    if (event.type == SDL_MOUSEBUTTONDOWN)
    {
        for (i=0;i<max_objects;i++)
        {
            coord = position;
            deploy(
                    &coord,
                    determine_direction(characters[i].movement->position, coord),
                    characters[i].movement->formation, i
                    );
            if (is_pos_legal(coord, characters[i].movement->position, max_coord, map.collisions))
            {
                characters[i].movement->current_node = 0;
                characters[i].movement->path[0] = coord;
                if (event.button.button == SDL_BUTTON_LEFT)
                    fire_movement(characters[i].movement, PATH);
                else if (event.button.button == SDL_BUTTON_RIGHT)
                    fire_movement(characters[i].movement, TELEPORT);
            }
            else
                stop_movement(characters[i].movement);
        }
    }
}
