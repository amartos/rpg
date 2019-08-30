#include "movement.h"

void init_coord(Coord *coord)
{
    coord->x = 0;
    coord->y = 0;
}

static void reset_coord(Coord *coord)
{
    init_coord(coord);
}

static unsigned int is_same_coord(Coord a, Coord b)
{
    unsigned int same = FALSE;
    if (a.x == b.x && a.y == b.y)
        same = TRUE;
    return same;
}

static unsigned int is_colliding(
        Coord const goal,
        unsigned int** const collision_map
        )
{
    /* As the collision box is located on the feet, add the x and y coord to
     * obtain the true (0,0) corner of the image, then check */
    unsigned int x = (goal.x + COLLISION_BOX_OFFSET_X) / TILES_WIDTH;
    unsigned int y = (goal.y + COLLISION_BOX_OFFSET_Y) / TILES_HEIGHT;
    unsigned int collide = FALSE;
    collide = collision_map[x][y];

    return collide;
}

static unsigned int is_out_of_map(Coord const goal, Coord const max_coord)
{
    unsigned int is_out = FALSE;
    unsigned int x = goal.x / TILES_WIDTH, y = goal.y / TILES_HEIGHT;

    if (goal.x < 0 || goal.y < 0 || x >= max_coord.x || y >= max_coord.y)
        is_out = TRUE;

    return is_out;
}

static void teleport(Coord *start, Coord *goal)
{
    *start = *goal;
}

static void walk(Coord *start, Coord *goal, int const velocity)
{
    int Dx, Dy;
    Dx = start->x - goal->x;
    if (Dx < 0)
        start->x += velocity;
    else if (Dx > 0)
        start->x -= velocity;

    Dy = start->y - goal->y;
    if (Dy < 0)
        start->y += velocity;
    else if (Dy > 0)
        start->y -= velocity;
}

void move(
        Coord *start,
        Coord *goal,
        MovementType const type,
        Coord const max_coord,
        unsigned int** const collision_map,
        int const velocity
        )
{
    if (
            !is_same_coord(*start, *goal) &&
            !is_colliding(*goal, collision_map) &&
            !is_out_of_map(*goal, max_coord)
        )
    {
        switch (type)
        {
            case WALK:
                walk(start, goal, velocity);
                break;
            case TELEPORT:
                teleport(start, goal);
                break;
        }
    }

    if (is_same_coord(*start, *goal))
        reset_coord(goal);
}
