#include "movement.h"


static void teleport(Movement *movement)
{
    movement->direction = determine_direction(
            movement->position,
            movement->path[movement->current_node]
            );
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
    deploy(movement, id);
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
    movement->direction = determine_direction(
            movement->position,
            movement->path[movement->current_node]
            );
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

static int round_angle(float n)
{
    int x = 0;
    if (n <= -0.5)
        x = floor(n);
    else if (n >= 0.5)
        x = ceil(n);
    else
        x = 0;

    return x;
}

static float sind(unsigned int angle)
{
    return sin(angle*M_PI/180);
}

static float cosd(unsigned int angle)
{
    return cos(angle*M_PI/180);
}

void deploy(Movement *movement, unsigned int const char_number)
{
    // most of this funtion will depend on the MAX_CHARACTERS, but cannot be
    // linked as it is very specific, thus need to be independently defined
    unsigned space = TILES_WIDTH/4, absox, absoy;
    int ox = 0, oy = 0;
    unsigned int half_cn = char_number/2, third_cn = char_number/3;
    Cardinals direction = movement->direction;

    switch(movement->formation)
    {
        case LINE:
            ox = round_angle(cosd(270 + 45*direction)) * (3 - char_number);
            oy = round_angle(sind(270 + 45*direction)) * (3 - char_number);
            break;
        case SQUARE:
            ox = round_angle(cosd(135+90*char_number + 45*direction));
            oy = round_angle(sind(135+90*char_number + 45*direction));
            break;
        case TRIANGLE:
            if (char_number)
            {
                ox = round_angle(cosd(270 - half_cn * 135 - third_cn * 90 + 45*direction));
                oy = round_angle(sind(270 - half_cn * 135 - third_cn * 90 + 45*direction));
            }
            break;
        case CIRCLE:
            ox = round_angle(cosd(180 + 45 * char_number + half_cn * 45 + 45*direction));
            oy = round_angle(sind(180 + 45 * char_number + half_cn * 45 + 45*direction));
            if (char_number == 0)
                ox -= 1;
            else if (char_number == 3)
                ox += 1;
            break;
        default:
            ox = 0;
            oy = 0;
            break;
        }
    ox *= TILES_WIDTH;
    oy *= TILES_HEIGHT;

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
