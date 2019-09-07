#include "movement.h"


static void teleport(Movement *movement)
{
    // round coord to current tile, not *exact* click position
    round_coord(&movement->path[0]);
    movement->position = movement->path[movement->current_node];
    reset_coord(&movement->path[movement->current_node]);
}

void init_movement(
        Movement *movement,
        Coord const start_position,
        Deployment const formation,
        unsigned int const id
        )
{
    unsigned int i;
    init_coord(&movement->position);
    for (i=0;i<MAX_PATH_NODES;i++)
        init_coord(&movement->path[i]);
    movement->path[0] = start_position;
    movement->moving = FALSE;
    movement->velocity = 4;
    movement->direction = S;
    movement->current_node = 0;
    movement->movement_type = WALK;

    movement->formation = formation;
    formation_offsetting(movement, id);
    teleport(movement); // place character in start_position
}

static Coord determine_decrease(Movement const movement)
{
    unsigned int Dx = abs(movement.position.x - movement.path[movement.current_node].x);
    unsigned int Dy = abs(movement.position.y - movement.path[movement.current_node].y);
    Coord decrease; init_coord(&decrease);

    if (Dy < movement.velocity)
        decrease.y = movement.velocity - Dy;
    if (Dx < movement.velocity)
        decrease.x = movement.velocity - Dx;

    return decrease;
}

static void walk(Movement *movement)
{
    Coord decrease = determine_decrease(*movement);

    switch(movement->direction)
    {
        case N:
            movement->position.y -= movement->velocity - decrease.y;
            break;
        case NE:
            movement->position.x += movement->velocity - decrease.x;
            movement->position.y -= movement->velocity - decrease.y;
            break;
        case E:
            movement->position.x += movement->velocity - decrease.x;
            break;
        case SE:
            movement->position.x += movement->velocity - decrease.x;
            movement->position.y += movement->velocity - decrease.y;
            break;
        case S:
            movement->position.y += movement->velocity - decrease.y;
            break;
        case SW:
            movement->position.x -= movement->velocity - decrease.x;
            movement->position.y += movement->velocity - decrease.y;
            break;
        case W:
            movement->position.x -= movement->velocity - decrease.x;
            break;
        case NW:
            movement->position.x -= movement->velocity - decrease.x;
            movement->position.y -= movement->velocity - decrease.y;
            break;
    }
}

void formation_offsetting(Movement *movement, unsigned int const char_number)
{
    // most of this funtion will depend on the MAX_CHARACTERS, but cannot be
    // linked as it is very specific, thus need to be independently defined
    unsigned space = TILES_WIDTH/4, absox, absoy;
    int ox = 0, oy = 0;

    switch(movement->formation)
    {
        case LINE:
            ox = 0;
            oy = char_number * TILES_HEIGHT + space;
            break;
        case SQUARE:
            switch(char_number)
            {
                case 0:
                    ox = -1 * TILES_WIDTH;
                    oy = TILES_HEIGHT;
                    break;
                case 1:
                    ox = -1 * TILES_WIDTH;
                    oy = -1 * TILES_HEIGHT;
                    break;
                case 2:
                    ox = TILES_WIDTH;
                    oy = -1 * TILES_HEIGHT;
                    break;
                case 3:
                    ox = TILES_WIDTH;
                    oy = TILES_HEIGHT;
                    break;
            }
            break;
        case TRIANGLE:
            switch(char_number)
            {
                case 0:
                    ox = 0;
                    oy = 0;
                    break;
                case 1:
                    ox = 0;
                    oy = -1 * (TILES_HEIGHT + space);
                    break;
                case 2:
                    ox = -1 * TILES_WIDTH;
                    oy = TILES_HEIGHT;
                    break;
                case 3:
                    ox = TILES_WIDTH;
                    oy = TILES_HEIGHT;
                    break;
            }
            break;
        case CIRCLE:
            switch(char_number)
            {
                case 0:
                    ox = -2 * TILES_WIDTH;
                    oy = TILES_HEIGHT;
                    break;
                case 1:
                    ox = -1 * TILES_WIDTH;
                    oy = 0;
                    break;
                case 2:
                    ox = 0;
                    oy = 0;
                    break;
                case 3:
                    ox = TILES_WIDTH;
                    oy = TILES_HEIGHT;
                    break;
            }
            break;
        default:
            ox = 0;
            oy = 0;
            break;
        }

    absox = abs(ox); absoy = abs(oy);

    if (absox > movement->path[movement->current_node].x && ox < 0)
        movement->path[movement->current_node].x = 0;
    else
        movement->path[movement->current_node].x += ox;

    if (absoy > movement->path[movement->current_node].y && oy < 0)
        movement->path[movement->current_node].y = 0;
    else
        movement->path[movement->current_node].y += oy;
}

void move(
        Movement *movement,
        Coord const max_coord,
        unsigned int** const collision_map,
        unsigned int** const cost_map
        )
{
    unsigned int i, nodes = 0;
    Coord start; init_coord(&start);

    if (
            !is_same_coord(movement->position, movement->path[movement->current_node]) &&
            !is_colliding(movement->path[movement->current_node], collision_map, TRUE) &&
            !is_out_of_map(movement->path[movement->current_node], max_coord)
        )
    {
        switch (movement->movement_type)
        {
            case PATH:
                // these 2 lines avoid the conversion of the pixels position
                start = movement->position;
                pixels_to_unit(&start);

                pixels_to_unit(&movement->path[movement->current_node]);
                movement->movement_type = WALK;
                nodes = find_path(
                        movement->path,
                        start,
                        movement->path[movement->current_node],
                        max_coord.x, max_coord.y,
                        collision_map,
                        cost_map,
                        NULL
                        );
                if (nodes)
                {
                    movement->current_node = nodes - 1;
                    for (i=0;i<nodes;i++)
                        unit_to_pixels(&movement->path[i]);
                    goto walking;
                }
                else
                    goto end_move;
            case WALK:
                goto walking;
            case TELEPORT:
                teleport(movement);
                goto end_move;
            walking:
                movement->direction = determine_direction(
                        movement->position,
                        movement->path[movement->current_node]
                        );
                walk(movement);
                if (is_same_coord(movement->position, movement->path[movement->current_node]))
                {
                    if (!movement->current_node)
                        goto end_move;
                    else
                    {
                        movement->current_node--;
                        break;
                    }
                }
                else
                    break;
            end_move:
                movement->current_node = 0;
                reset_coord(&movement->path[0]);
                movement->movement_type = WALK;
                movement->moving = FALSE;
                break;
        }
    }
    else
    {
        movement->current_node = 0;
        reset_coord(&movement->path[0]);
        movement->movement_type = WALK;
        movement->moving = FALSE;
    }
}
