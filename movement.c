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

void init_movement(Movement *movement)
{
    unsigned int i;
    init_coord(&movement->position);
    for (i=0;i<MAX_PATH_NODES;i++)
        init_coord(&movement->path[i]);
    movement->moving = FALSE;
    movement->velocity = 0;
    movement->direction = S;
    movement->current_node = 0;
    movement->movement_type = WALK;
    movement->formation = LINE;
}

static Coord determine_decrease(Movement const movement)
{
    Coord decrease; init_coord(&decrease);
    Coord goal; init_coord(&goal);
    goal.x = fabs(movement.position.x - movement.path[movement.current_node].x);
    goal.y = fabs(movement.position.y - movement.path[movement.current_node].y);

    decrease.x = fmod(goal.x, movement.velocity);
    decrease.y = fmod(goal.y, movement.velocity);

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

static double round_angle(float n)
{
    double x = 0.0;
    if (n <= -0.5)
        x = floor(n);
    else if (n >= 0.5)
        x = ceil(n);
    else
        x = 0.0;

    return x;
}

static double sind(unsigned int angle)
{
    return sin(angle*M_PI/180);
}

static double cosd(unsigned int angle)
{
    return cos(angle*M_PI/180);
}

void change_formation(Movement *movement, Deployment formation)
{
    movement->formation = formation;
    fire_movement(movement, PATH);
}

void deploy(
        Coord *position,
        Cardinals const direction,
        Deployment const formation,
        unsigned int const char_number,
        Camera const camera
        )
{
    // most of this funtion will depend on the MAX_CHARACTERS, but cannot be
    // linked as it is very specific, thus need to be independently defined
    double space = TILES_WIDTH/4, absox, absoy;
    double ox = 0, oy = 0;
    double half_cn = char_number/2, third_cn = char_number/3;

    if (char_number)
    {
        switch(formation)
        {
            case LINE:
                ox = round_angle(cosd(270 + 45*direction)) * (-1 * char_number);
                oy = round_angle(sind(270 + 45*direction)) * (-1 * char_number);
                break;
            case SQUARE:
                ox = round_angle(cosd(270 + 45*char_number + 45*direction));
                oy = round_angle(sind(270 + 45*char_number + 45*direction));
                break;
            case TRIANGLE:
                ox = round_angle(cosd(270 - half_cn * 135 - third_cn * 90 + 45*direction));
                oy = round_angle(sind(270 - half_cn * 135 - third_cn * 90 + 45*direction));
                break;
            case CIRCLE:
                ox = round_angle(cosd(270 + 45*char_number + 45*direction)) + 1 + third_cn;
                oy = round_angle(sind(270 + 45*char_number + 45*direction));
                break;
            default:
                ox = 0.0;
                oy = 0.0;
                break;
            }

        absox = fabs(ox); absoy = fabs(oy);

        if (absox > position->x && ox < 0)
            position->x = 0;
        else
            position->x += ox;

        if (absoy > position->y && oy < 0)
            position->y = 0;
        else
            position->y += oy;
    }
}

void fire_movement(Movement *movement, MovementType const movement_type)
{
    movement->moving = TRUE;
    movement->movement_type = movement_type;
}

void stop_movement(Movement *movement)
{
    movement->current_node = 0;
    reset_coord(&movement->path[0]);
    movement->movement_type = WALK;
    movement->moving = FALSE;
}

void move(
        Movement *movement,
        Coord const max_coord,
        unsigned int** const collision_map,
        unsigned int** const cost_map
        )
{
    unsigned int nodes = 0;
    Coord start; init_coord(&start);

    switch (movement->movement_type)
    {
        case PATH:
            movement->movement_type = WALK;
            nodes = find_path(
                    movement->path,
                    movement->position,
                    movement->path[movement->current_node],
                    max_coord,
                    collision_map,
                    cost_map,
                    NULL
                    );
            if (nodes)
            {
                movement->current_node = nodes - 1;
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
                    movement->current_node--;
            }
            break;
        end_move:
            stop_movement(movement);
            break;
    }
}
