#include "event_handler.h"


void handle_keyboard(
        SDL_Event event, Bool *paused,
        Character characters[MAX_CHARACTERS],
        Coord *scroll
        )
{
    switch (event.key.keysym.sym)
    {
        case SDLK_SPACE:
            *paused = !*paused;
            break;
        case SDLK_a:
            change_formation(characters, LINE);
            break;
        case SDLK_z:
            change_formation(characters, SQUARE);
            break;
        case SDLK_e:
            change_formation(characters, TRIANGLE);
            break;
        case SDLK_r:
            change_formation(characters, CIRCLE);
            break;
        case SDLK_F5:
            scroll->x = (SCREEN_WIDTH/2)/TILES_WIDTH;
            scroll->y = (SCREEN_HEIGHT/2)/TILES_HEIGHT;
            break;
        case SDLK_UP:
            scroll->y -= 1;
            break;
        case SDLK_DOWN:
            scroll->y += 1;
            break;
        case SDLK_LEFT:
            scroll->x -= 1;
            break;
        case SDLK_RIGHT:
            scroll->x += 1;
            break;
    }
}

Cursors handle_mouse_motion(
        SDL_Event const event,
        SDL_Rect *mouse_hover_rect,
        Coord const max_coord
        )
{
    Coord position = event_to_coord(event.motion.x, event.motion.y);
    *mouse_hover_rect = coord_to_isosdlrect(position);
    if (!is_out_of_map(position, max_coord))
        return HOVER;
    else
        return INVALID;
}

void handle_mouse_click(
        SDL_Event event,
        Coord const scroll,
        Character characters[MAX_CHARACTERS],
        Map const map
        )
{
    unsigned int i;

    Coord max_coord; init_coord(&max_coord);
    max_coord.x = map.maxx; max_coord.y = map.maxy;

    Coord position = event_to_coord(event.button.x, event.button.y);
    position.x += scroll.x; position.y += scroll.y;

    Coord coord; init_coord(&coord);

    if (event.type == SDL_MOUSEBUTTONDOWN)
    {
        for (i=0;i<MAX_CHARACTERS;i++)
        {
            coord = position;
            deploy(
                    &coord,
                    determine_direction(characters[i].movement.position, coord),
                    characters[i].movement.formation, i
                    );
            if (is_pos_legal(coord, characters[i].movement.position, max_coord, map.collisions))
            {
                characters[i].movement.current_node = 0;
                characters[i].movement.path[0] = coord;
                if (event.button.button == SDL_BUTTON_LEFT)
                    fire_movement(&characters[i].movement, PATH);
                else if (event.button.button == SDL_BUTTON_RIGHT)
                    fire_movement(&characters[i].movement, TELEPORT);
            }
            else
                stop_movement(&characters[i].movement);
        }
    }
}
