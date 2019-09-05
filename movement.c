#include "movement.h"


void init_movement(
        Movement *movement,
        Coord const start_position,
        Deployment const formation,
        unsigned int const id
        )
{
    unsigned int i;
    init_coord(&movement->position);
    movement->position = start_position;
    formation_offsetting(&movement->position, id, formation);
    movement->formation = formation;
    for (i=0;i<MAX_PATH_NODES;i++)
        init_coord(&movement->path[i]);
    movement->moving = FALSE;
    movement->velocity = 4;
    movement->direction = S;
    movement->current_node = 0;
    movement->movement_type = WALK;
}

static void teleport(Coord *start, Coord const goal)
{
    *start = goal;
}

static Cardinals determine_direction(Coord const start, Coord const goal)
{
    Cardinals direction = S;
    int Dx, Dy;

    Dx = start.x - goal.x;
    Dy = start.y - goal.y;

    if (Dx < 0 && Dy < 0)
        direction = SE;
    else if (Dx < 0 && Dy == 0)
        direction = E;
    else if (Dx < 0 && Dy > 0)
        direction = NE;
    else if (Dx == 0 && Dy < 0)
        direction = S;
    else if (Dx == 0 && Dy > 0)
        direction = N;
    else if (Dx > 0 && Dy < 0)
        direction = SW;
    else if (Dx > 0 && Dy == 0)
        direction = W;
    else if (Dx > 0 && Dy > 0)
        direction = NW;


    return direction;
}

static Coord determine_decrease(Coord const start, Coord const goal, unsigned int const velocity)
{
    unsigned int Dx = abs(start.x - goal.x);
    unsigned int Dy = abs(start.y - goal.y);
    Coord decrease; init_coord(&decrease);

    if (Dy < velocity)
        decrease.y = velocity - Dy;
    if (Dx < velocity)
        decrease.x = velocity - Dx;

    return decrease;
}

static Cardinals walk(Coord *start, Coord const goal, unsigned int const velocity)
{
    Cardinals direction = determine_direction(*start, goal);
    Coord decrease = determine_decrease(*start, goal, velocity);

    switch(direction)
    {
        case N:
            start->y -= velocity - decrease.y;
            break;
        case NE:
            start->x += velocity - decrease.x;
            start->y -= velocity - decrease.y;
            break;
        case E:
            start->x += velocity - decrease.x;
            break;
        case SE:
            start->x += velocity - decrease.x;
            start->y += velocity - decrease.y;
            break;
        case S:
            start->y += velocity - decrease.y;
            break;
        case SW:
            start->x -= velocity - decrease.x;
            start->y += velocity - decrease.y;
            break;
        case W:
            start->x -= velocity - decrease.x;
            break;
        case NW:
            start->x -= velocity - decrease.x;
            start->y -= velocity - decrease.y;
            break;
    }
    return direction;
}

void formation_offsetting(Coord *position, unsigned int const char_number, Deployment const deployment)
{
    // most of this funtion will depend on the MAX_CHARACTERS, but cannot be
    // linked as it is very specific, thus need to be independently defined
    unsigned space = TILES_WIDTH/4, absox, absoy;
    int ox = 0, oy = 0;

    switch(deployment)
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

    if (absox > position->x && ox < 0)
        position->x = 0;
    else
        position->x += ox;

    if (absoy > position->y && oy < 0)
        position->y = 0;
    else
        position->y += oy;
}

void move(
        Movement *movement,
        Coord const max_coord,
        unsigned int** const collision_map,
        unsigned int** const cost_map
        )
{
    unsigned int i;
    Coord start = movement->position;
    Coord goal = movement->path[movement->current_node];
    unsigned int nodes = 0;

    if (
            !is_same_coord(start, goal) &&
            !is_colliding(goal, collision_map, TRUE) &&
            !is_out_of_map(goal, max_coord)
        )
    {
        switch (movement->movement_type)
        {
            case PATH:
                pixels_to_unit(&start);
                pixels_to_unit(&goal);
                movement->movement_type = WALK;
                nodes = find_path(
                        movement->path,
                        start, goal,
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
                    goal = movement->path[movement->current_node];
                    goto walking;
                }
                else
                    goto end_move;
            case WALK:
                goto walking;
            case TELEPORT:
                // ensure that its not TP to a different node than the click
                goal = movement->path[0]; round_coord(&goal);
                teleport(&movement->position, goal);
                goto end_move;
            walking:
                movement->direction = walk(&movement->position, goal, movement->velocity);
                if (is_same_coord(movement->position, goal))
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
