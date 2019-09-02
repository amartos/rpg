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
    Coord oa = offsetting(a);
    Coord ob = offsetting(b);
    if (oa.x == ob.x && oa.y == ob.y)
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
    Coord goal_offset = offsetting(goal);

    if (pixel)
    {
        // As the collision box is located on the feet, add the x and y coord to
        // obtain the true (0,0) corner of the image, then check
        x = (goal_offset.x + COLLISION_BOX_OFFSET_X) / TILES_WIDTH;
        y = (goal_offset.y + COLLISION_BOX_OFFSET_Y) / TILES_HEIGHT;
    }
    else
    {
        x = goal_offset.x;
        y = goal_offset.y;
    }

    collide = collision_map[x][y];
    return collide;
}

static Bool is_out_of_map(Coord const goal, Coord const max_coord)
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
    Coord coord_offset = offsetting(coord);
    unsigned int number = max_coord.x * coord_offset.y + coord_offset.x;
    return number;
}

static void get_neighbours(Coord next[8], Coord const current, Coord const max_coord)
{
    Coord current_offset = offsetting(current);
    unsigned int i;

    next[N].x = current.x;
    next[S].x = current.x;
    next[E].y = current.y;
    next[W].y = current.y;
    for (i=W;i<=SW;i++)
    {
        next[i].ox = current.ox;
        next[i].oy = current.oy;
    }

    if (current_offset.y <= 0) //edge
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

    if (current_offset.y == max_coord.y-1) // edge
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

    if (current_offset.x == (max_coord.x-1)) // edge
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

    if (current_offset.x <= 0) // edge
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
    Coord next_coord_offset = offsetting(next_coord);
    Coord goal_offset = offsetting(goal);
    unsigned int next_cost = 0;
    unsigned int x = next_coord_offset.x, y = next_coord_offset.y;
    int distance = abs(goal_offset.x - x) + abs(goal_offset.y - y);

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
    
    start = start_pixels;
    start.x /= TILES_WIDTH;
    start.y /= TILES_HEIGHT;
    start.ox /= TILES_WIDTH;
    start.oy /= TILES_HEIGHT;

    goal = goal_pixels;
    goal.x /= TILES_WIDTH;
    goal.y /= TILES_HEIGHT;
    goal.ox /= TILES_WIDTH;
    goal.oy /= TILES_HEIGHT;

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

    for (i=W;i<=SW;i++)
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
                for (i=W;i<=SW;i++)
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
        current.ox = goal.ox;
        current.oy = goal.oy;
        next.ox = goal.ox;
        next.oy = goal.oy;
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

            (*path)[i] = current;
            ncurrent = nnext;
        }

        nodes--;
    }

    return nodes;
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

Cardinals move(
        Coord *start,
        Coord const goal,
        MovementType const type,
        Coord const max_coord,
        unsigned int** const collision_map,
        unsigned int const velocity
        )
{
    Cardinals direction = S;
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
