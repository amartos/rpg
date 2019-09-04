#include "movement.h"


void init_coord(Coord *coord)
{
    coord->x = 0;
    coord->y = 0;
}

void reset_coord(Coord *coord)
{
    init_coord(coord);
}

Bool is_same_coord(Coord const a, Coord const b)
{
    Bool same = FALSE;
    if (a.x == b.x && a.y == b.y)
        same = TRUE;
    return same;
}

Bool is_colliding(
        Coord goal,
        unsigned int** const collision_map,
        Bool const pixel
        )
{
    unsigned int xl, yu, xr, yd;

    if (!pixel)
    {
        goal.x *= TILES_WIDTH;
        goal.y *= TILES_HEIGHT;
    }

    // As the collision box is located on the feet, add the x and y collision
    // coord to obtain the true (0,0) corner of the image.
    // check also the right side by adding the collision box w/h
    xl = (goal.x + COLLISION_BOX_OFFSET_X) / TILES_WIDTH;
    yu = (goal.y + COLLISION_BOX_OFFSET_Y) / TILES_HEIGHT;

    xr = (goal.x + COLLISION_BOX_OFFSET_X + COLLISION_BOX_WIDTH) / TILES_WIDTH;
    yd = (goal.y + COLLISION_BOX_OFFSET_Y + COLLISION_BOX_HEIGHT) / TILES_HEIGHT;
    
    if (collision_map[xl][yu] || collision_map[xr][yd]) // up left || down right corners
        return TRUE;
    else
        return FALSE;
}

Bool is_out_of_map(Coord goal, Coord const max_coord)
{
    Bool is_out = FALSE;
    int x = goal.x / TILES_WIDTH, y = goal.y / TILES_HEIGHT;
    int mx = max_coord.x, my = max_coord.y;

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

    if (char_number < MAX_CHARACTERS)
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
            case NONE:
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
