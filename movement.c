#include "movement.h"


void init_coord(Coord *coord)
{
    coord->x = 0;
    coord->y = 0;
    coord->ox = 0;
    coord->oy = 0;
}

Bool is_same_coord(Coord const a, Coord const b)
{
    Bool same = FALSE;
    if (a.x == b.x && a.y == b.y)
        same = TRUE;
    return same;
}

static Bool is_colliding(
        Coord const goal,
        unsigned int** const collision_map,
        Bool const pixel
        )
{
    unsigned int x, y;
    Bool collide = FALSE;

    if (pixel)
    {
        // As the collision box is located on the feet, add the x and y coord to
        // obtain the true (0,0) corner of the image, then check
        x = (goal.x + COLLISION_BOX_OFFSET_X) / TILES_WIDTH;
        y = (goal.y + COLLISION_BOX_OFFSET_Y) / TILES_HEIGHT;
    }
    else
    {
        x = goal.x;
        y = goal.y;
    }

    collide = collision_map[x][y];
    return collide;
}

static Bool is_out_of_map(Coord const goal, Coord const max_coord)
{
    Bool is_out = FALSE;
    unsigned int x = goal.x, y = goal.y;

    if (goal.x < 0 || goal.y < 0 || x >= max_coord.x || y >= max_coord.y)
        is_out = TRUE;

    return is_out;
}

static void teleport(Coord *start, Coord const goal)
{
    start->x = goal.x;
    start->y = goal.y;
}

static Direction walk(Coord *start, Coord const goal, unsigned int const velocity)
{
    Direction direction = DOWN;
    int Dx, Dy;

    Dy = start->y - goal.y;
    if (Dy < 0)
    {
        direction = DOWN;
        start->y += velocity;
    }
    else if (Dy > 0)
    {
        direction = UP;
        start->y -= velocity;
    }

    Dx = start->x - goal.x;
    if (Dx < 0)
    {
        direction = RIGHT;
        start->x += velocity;
    }
    else if (Dx > 0)
    {
        direction = LEFT;
        start->x -= velocity;
    }

    return direction;
}

// A* pathfinding

static void set_in_queue(unsigned int const max_array, unsigned int queue[], unsigned int const next)
{
    unsigned int i;
    Bool already_in = FALSE;
    for (i=0;i<max_array;i++)
        if (queue[i] == next)
            already_in = TRUE;
    if (!already_in)
        for (i=0;i<max_array;i++)
            if (!queue[i])
            {
                queue[i] = next;
                break;
            }
}

static void delete_from_queue(unsigned int const max_array, unsigned int queue[], unsigned int const number)
{
    unsigned int i;
    for (i=0;i<max_array;i++)
        if (queue[i] == number)
            queue[i] = 0;
}

static Bool check_queue(unsigned int const max_array, unsigned int const queue[])
{
    unsigned i;
    Bool done = TRUE;
    for (i=0;i<max_array;i++)
        if (queue[i])
            done = FALSE;
    return done;
}

static Bool is_queue_ordered(unsigned int const max_array, unsigned int const queue[], unsigned int const cost[])
{
    unsigned int i, current, next;
    Bool done = TRUE;
    for (i=0;i<max_array-1;i++)
    {
        current = queue[i];
        next = queue[i+1];
        if (next)
            if (cost[current] > cost[next] || !current)
               done = FALSE;
    }
    return done;
}

static void order_queue(unsigned int const max_array, unsigned int queue[], unsigned int const cost[])
{
    unsigned int i, current, next;
    Bool done = FALSE;

    while (!done)
    {
        for (i=0;i<max_array-1;i++) // do not check last+1
        {
            current = queue[i];
            next = queue[i+1];
            if (next)
                if (cost[current] > cost[next] || !current)
                {
                    queue[i] = next;
                    queue[i+1] = current;
                }
        }
        done = is_queue_ordered(max_array, queue, cost);
    }
}

static unsigned int convert_coord_to_number(Coord const coord, Coord const max_coord)
{
    unsigned int number = max_coord.x * coord.y + coord.x;
    return number;
}

static void get_neighbours(Coord next[8], Coord const current, Coord const max_coord)
{
    next[N].x = current.x;
    next[S].x = current.x;
    next[E].y = current.y;
    next[W].y = current.y;

    if (current.y <= 0) //edge
    {
        next[N].y = current.y;
        next[NE].y = current.y;
        next[NW].y = current.y;
    }
    else
    {
        next[N].y = current.y - 1;
        next[NE].y = current.y - 1;
        next[NW].y = current.y - 1;
    }

    if (current.y == max_coord.y-1) // edge
    {
        next[S].y = current.y;
        next[SE].y = current.y;
        next[SW].y = current.y;
    }
    else
    {
        next[S].y = current.y + 1;
        next[SE].y = current.y + 1;
        next[SW].y = current.y + 1;
    }

    if (current.x == (max_coord.x-1)) // edge
    {
        next[E].x = current.x;
        next[NE].x = current.x;
        next[SE].x = current.x;
    }
    else
    {
        next[E].x = current.x + 1;
        next[NE].x = current.x + 1;
        next[SE].x = current.x + 1;
    }

    if (current.x <= 0) // edge
    {
        next[W].x = current.x;
        next[NW].x = current.x;
        next[SW].x = current.x;
    }
    else
    {
        next[W].x = current.x - 1;
        next[NW].x = current.x - 1;
        next[SW].x = current.x - 1;
    }
}

static unsigned int calculate_cost(
        unsigned int current_cost,
        Coord const next_coord,
        Coord const goal,
        unsigned int** const cost_map
        )
{
    unsigned int next_cost = 0;
    unsigned int x = next_coord.x, y = next_coord.y;
    int distance = abs(goal.x - next_coord.x) + abs(goal.y - next_coord.y);

    next_cost = current_cost + distance + cost_map[x][y];
    return next_cost;
}

unsigned int find_path(
        Coord *(*path),
        Coord const start_pixels,
        Coord goal_pixels,
        unsigned int const velocity,
        Coord const max_coord_pixels,
        unsigned int** const collision_map,
        unsigned int** const movement_cost_map
        )
{
    // vars init
    unsigned int nodes = 0;

    Coord *tmp = NULL;
    Coord start; init_coord(&start);
    Coord goal; init_coord(&goal);
    Coord max_coord; init_coord(&max_coord);

    start.x = start_pixels.x / TILES_WIDTH;
    start.y = start_pixels.y / TILES_HEIGHT;
    goal.x = goal_pixels.x / TILES_WIDTH;
    goal.y = goal_pixels.y / TILES_HEIGHT;
    max_coord.x = max_coord_pixels.x / TILES_WIDTH;
    max_coord.y = max_coord_pixels.y / TILES_HEIGHT;

    // all this can be put in the first if statement for optimisation
    unsigned int i = 0, j = 0, n = 0, nnext = 0, ncurrent = 0;
    unsigned int nstart = convert_coord_to_number(start, max_coord);
    unsigned int ngoal = convert_coord_to_number(goal, max_coord);
    unsigned int max_array = max_coord.x * max_coord.y;
    unsigned int queue[max_array], cost[max_array+1], came_from[max_array+1];
    unsigned int new_cost = 0;
    Bool done = FALSE;

    Coord current; init_coord(&current);
    Coord next; init_coord(&next);
    Coord coord; init_coord(&coord);
    Coord all_next[8];
    Coord conversion[max_array+1];

    for (i=N;i<=SW;i++)
        init_coord(&all_next[i]);
    for (i=0;i<max_array;i++)
        queue[i] = 0;
    for (i=0;i<max_array+1;i++)
    {
        init_coord(&conversion[i]);
        cost[i] = 0;
        came_from[i] = 0;
    }
    for (i=0;i<max_coord.x;i++)
        for (j=0;j<max_coord.y;j++)
        {
            coord.x = i, coord.y = j;
            n = convert_coord_to_number(coord, max_coord);
            conversion[n] = coord;
        }

    came_from[nstart] = nstart;
    queue[0] = nstart;
    cost[nstart] = 0;

    // A* starts here
    if (
            !is_same_coord(start, goal) &&
            !is_colliding(goal, collision_map, FALSE) &&
            !is_out_of_map(goal, max_coord)
        )
    {
        while (!done)
        {
            ncurrent = queue[0];
            if (ncurrent)
            {
                delete_from_queue(max_array, queue, ncurrent);
                current = conversion[ncurrent]; // get coord from number
                get_neighbours(all_next, current, max_coord);
                for (i=N;i<=SW;i++)
                {
                    if (
                        !is_same_coord(start, all_next[i]) &&
                        !is_colliding(all_next[i], collision_map, FALSE)
                        )
                    {
                        nnext = convert_coord_to_number(all_next[i], max_coord);
                        new_cost = calculate_cost(cost[ncurrent], all_next[i], goal, movement_cost_map);
                        if (!came_from[nnext] || new_cost < cost[nnext])
                        {
                            came_from[nnext] = ncurrent;
                            cost[nnext] = new_cost;
                            set_in_queue(max_array, queue, nnext);
                        }
                        else
                            delete_from_queue(max_array, queue, nnext);

                        if (nnext == ngoal)
                        {
                            done = TRUE;
                            break;
                        }
                    }
                }

                if (!done)
                {
                    order_queue(max_array, queue, cost);
                    done = check_queue(max_array, queue);
                }
            }
            else
                done = TRUE;
        }

        ncurrent = ngoal;
        while (ncurrent != nstart)
        {
            nodes++;
            ncurrent = came_from[ncurrent]; // came_from[goal] = previous
            if (nodes > max_array || ncurrent == 0)
                return 0; // if no nodes found, return 0 nodes
        }

        TRY
        {
            tmp = (Coord *)realloc(*path, sizeof(Coord) * nodes);
            if (tmp == NULL)
                THROW(PATHFIND_MALLOC_FAILURE);
            else
                *path = tmp;
        }
        CATCH(PATHFIND_MALLOC_FAILURE)
        {
            logger(PATHFIND_MALLOC_FAILURE, "oups");
            exit(EXIT_FAILURE);
        }
        ETRY;

        ncurrent = ngoal;
        for (i=0;i<nodes;i++)
        {
            nnext = came_from[ncurrent];
            current.x = conversion[ncurrent].x * TILES_WIDTH;
            current.y = conversion[ncurrent].y * TILES_HEIGHT;
            next.x = conversion[nnext].x * TILES_WIDTH;
            next.y = conversion[nnext].y * TILES_HEIGHT;

            if (abs(current.x - next.x) % velocity)
                current.x += abs(current.x - next.x) % velocity;
            if (abs(current.y - next.y) % velocity)
                current.y += abs(current.y - next.y) % velocity;

            (*path)[i].x = current.x;
            (*path)[i].y = current.y;
            ncurrent = nnext;
        }

        nodes--;
    }

    return nodes;
}

Coord offsetting(Coord const center, Coord const offset)
{
    Coord sum; init_coord(&sum);
    Coord absoffset; init_coord(&absoffset);

    absoffset.x = abs(offset.x);
    absoffset.y = abs(offset.y);

    if (absoffset.x > center.x && offset.x < 0)
        sum.x = 0;
    else
        sum.x = center.x + offset.x;


    if (absoffset.y > center.y && offset.y < 0)
        sum.y = 0;
    else
        sum.y = center.y + offset.y;


    return sum;
}

void get_formation_offset(Coord offset[MAX_CHARACTERS], Deployment const deployment)
{
    // most of this funtion will depend on the MAX_CHARACTERS, but cannot be
    // linked as it is very specific, thus need to be independently defined
    unsigned int i, space = TILES_WIDTH/4;
    switch(deployment)
    {
        case LINE:
            for (i=0;i<MAX_CHARACTERS;i++)
            {
                offset[i].x = 0;
                offset[i].y = i * TILES_HEIGHT + space;
            }
            break;
        case SQUARE:
            offset[0].x = -1 * TILES_WIDTH;
            offset[0].y = TILES_HEIGHT;

            offset[1].x = -1 * TILES_WIDTH;
            offset[1].y = -1 * TILES_HEIGHT;

            offset[2].x = TILES_WIDTH;
            offset[2].y = -1 * TILES_HEIGHT;

            offset[3].x = TILES_WIDTH;
            offset[3].y = TILES_HEIGHT;
            break;
        case TRIANGLE:
            offset[0].x = 0;
            offset[0].y = 0;

            offset[1].x = 0;
            offset[1].y = -1 * (TILES_HEIGHT + space);

            offset[2].x = -1 * TILES_WIDTH;
            offset[2].y = TILES_HEIGHT;

            offset[3].x = TILES_WIDTH;
            offset[3].y = TILES_HEIGHT;
            break;
        case CIRCLE:
            offset[0].x = -2 * TILES_WIDTH;
            offset[0].y = TILES_HEIGHT;

            offset[1].x = -1 * TILES_WIDTH;
            offset[1].y = 0;

            offset[2].x = 0;
            offset[2].y = 0;

            offset[3].x = TILES_WIDTH;
            offset[3].y = TILES_HEIGHT;
            break;
    }
}

Direction move(
        Coord *start,
        Coord const goal,
        MovementType const type,
        Coord const max_coord,
        unsigned int** const collision_map,
        unsigned int const velocity
        )
{
    Direction direction = DOWN;
    if (
            !is_same_coord(*start, goal) &&
            !is_colliding(goal, collision_map, TRUE) &&
            !is_out_of_map(goal, max_coord)
        )
    {
        switch (type)
        {
            case PATH:
            case WALK:
                direction = walk(start, goal, velocity);
                break;
            case TELEPORT:
                teleport(start, goal);
                break;
        }
    }

    return direction;
}
