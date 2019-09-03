#include "movement.h"


void init_coord(Coord *coord)
{
    coord->x = 0;
    coord->y = 0;
    coord->ox = 0;
    coord->oy = 0;
}

void reset_coord(Coord *coord)
{
    init_coord(coord);
}

Bool is_same_coord(Coord const a, Coord const b)
{
    Bool same = FALSE;
    Coord oa = offsetting(a);
    Coord ob = offsetting(b);
    if (oa.x == ob.x && oa.y == ob.y)
        same = TRUE;
    return same;
}

Bool is_colliding(
        Coord const goal,
        unsigned int** const collision_map,
        Bool const pixel
        )
{
    unsigned int xl, yu, xr, yd;
    Coord goal_offset = offsetting(goal);

    if (!pixel)
    {
        goal_offset.x *= TILES_WIDTH;
        goal_offset.y *= TILES_HEIGHT;
    }

    // As the collision box is located on the feet, add the x and y collision
    // coord to obtain the true (0,0) corner of the image.
    // check also the right side by adding the collision box w/h
    xl = (goal_offset.x + COLLISION_BOX_OFFSET_X) / TILES_WIDTH;
    yu = (goal_offset.y + COLLISION_BOX_OFFSET_Y) / TILES_HEIGHT;

    xr = (goal_offset.x + COLLISION_BOX_OFFSET_X + COLLISION_BOX_WIDTH) / TILES_WIDTH;
    yd = (goal_offset.y + COLLISION_BOX_OFFSET_Y + COLLISION_BOX_HEIGHT) / TILES_HEIGHT;
    
    if (collision_map[xl][yu] || collision_map[xr][yd]) // up left || down right corners
        return TRUE;
    else
        return FALSE;
}

Bool is_out_of_map(Coord const goal, Coord const max_coord)
{
    Bool is_out = FALSE;
    Coord goal_offset = offsetting(goal);
    int x = goal_offset.x, y = goal_offset.y, mx = max_coord.x, my = max_coord.y;

    if (x < 0 || y < 0 || x >= mx || y >= my)
        is_out = TRUE;

    return is_out;
}

static void teleport(Coord *start, Coord const goal)
{
    start->x = goal.x;
    start->y = goal.y;
}

static Cardinals determine_direction(Coord const start, Coord const goal)
{
    Cardinals direction = S;
    int Dx, Dy;
    Coord start_offset = offsetting(start);
    Coord goal_offset = offsetting(goal);

    Dy = start_offset.y - goal_offset.y;
    if (Dy < 0)
        direction = S;
    else if (Dy > 0)
        direction = N;

    Dx = start_offset.x - goal_offset.x;
    if (Dx < 0)
        direction = E;
    else if (Dx > 0)
        direction = W;

    return direction;
}

static Coord determine_decrease(Coord const start, Coord const goal, unsigned int const velocity)
{
    Coord start_offset = offsetting(start);
    Coord goal_offset = offsetting(goal);
    unsigned int Dx = abs(start_offset.x - goal_offset.x);
    unsigned int Dy = abs(start_offset.y - goal_offset.y);
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
        case W:
            start->x -= velocity - decrease.x;
            break;
        case E:
            start->x += velocity - decrease.x;
            break;
        case S:
            start->y += velocity - decrease.y;
            break;
        case N:
            start->y -= velocity - decrease.y;
            break;
        default:
            break;
    }
    return direction;
}

void copy_offset_from(Coord const original, Coord *copy)
{
    copy->ox = original.ox;
    copy->oy = original.oy;
}

Coord offsetting(Coord const position)
{
    Coord sum; init_coord(&sum);
    Coord absoffset; init_coord(&absoffset);

    absoffset.x = abs(position.ox);
    absoffset.y = abs(position.oy);

    if (absoffset.x > position.x && position.ox < 0)
        sum.x = 0;
    else
        sum.x = position.x + position.ox;


    if (absoffset.y > position.y && position.oy < 0)
        sum.y = 0;
    else
        sum.y = position.y + position.oy;


    return sum;
}

void get_formation_offset(Coord *position, unsigned int const char_number, Deployment const deployment)
{
    // most of this funtion will depend on the MAX_CHARACTERS, but cannot be
    // linked as it is very specific, thus need to be independently defined
    unsigned space = TILES_WIDTH/4;
    if (char_number < MAX_CHARACTERS)
        switch(deployment)
        {
            case LINE:
                position->ox = 0;
                position->oy = char_number * TILES_HEIGHT + space;
                break;
            case SQUARE:
                switch(char_number)
                {
                    case 0:
                        position->ox = -1 * TILES_WIDTH;
                        position->oy = TILES_HEIGHT;
                        break;
                    case 1:
                        position->ox = -1 * TILES_WIDTH;
                        position->oy = -1 * TILES_HEIGHT;
                        break;
                    case 2:
                        position->ox = TILES_WIDTH;
                        position->oy = -1 * TILES_HEIGHT;
                        break;
                    case 3:
                        position->ox = TILES_WIDTH;
                        position->oy = TILES_HEIGHT;
                        break;
                }
                break;
            case TRIANGLE:
                switch(char_number)
                {
                    case 0:
                        position->ox = 0;
                        position->oy = 0;
                        break;
                    case 1:
                        position->ox = 0;
                        position->oy = -1 * (TILES_HEIGHT + space);
                        break;
                    case 2:
                        position->ox = -1 * TILES_WIDTH;
                        position->oy = TILES_HEIGHT;
                        break;
                    case 3:
                        position->ox = TILES_WIDTH;
                        position->oy = TILES_HEIGHT;
                        break;
                }
                break;
            case CIRCLE:
                switch(char_number)
                {
                    case 0:
                        position->ox = -2 * TILES_WIDTH;
                        position->oy = TILES_HEIGHT;
                        break;
                    case 1:
                        position->ox = -1 * TILES_WIDTH;
                        position->oy = 0;
                        break;
                    case 2:
                        position->ox = 0;
                        position->oy = 0;
                        break;
                    case 3:
                        position->ox = TILES_WIDTH;
                        position->oy = TILES_HEIGHT;
                        break;
                }
                break;
        }
}

void move(Movement *movement, Coord const max_coord, unsigned int** const collision_map)
{
    Coord start = movement->position;
    unsigned int current_node = movement->current_node;
    Coord goal = movement->path[current_node];
    MovementType type = movement->movement_type;
    unsigned int velocity = movement->velocity;

    if (
            !is_same_coord(start, goal) &&
            !is_colliding(goal, collision_map, TRUE) &&
            !is_out_of_map(goal, max_coord)
        )
    {
        switch (type)
        {
            case PATH:
            case WALK:
                movement->direction = walk(&movement->position, goal, velocity);
                break;
            case TELEPORT:
                teleport(&movement->position, goal);
                break;
        }
        if (movement->current_node)
            movement->current_node--;
    }
}
