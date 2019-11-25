#include "pathfinding.h"


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
        unsigned int** const cost_map
        )
{
    unsigned int x = next_coord.x, y = next_coord.y;
    unsigned int next_cost = 1 + current_cost + cost_map[y][x];
    return next_cost;
}

static void dijkstra(
        Bool a_star,
        unsigned int cost[],
        unsigned int came_from[],
        Coord conversion[],
        Coord start, Coord goal,
        Coord const max_coord,
        unsigned int** const collision_map,
        unsigned int** const cost_map
        )
{
    unsigned int i = 0, nnext = 0, ncurrent = 0, new_cost = 0;
    unsigned int nstart = convert_coord_to_number(start, max_coord);
    unsigned int ngoal = convert_coord_to_number(goal, max_coord);
    unsigned int max_array = max_coord.x * max_coord.y;
    unsigned int queue[max_array];
    for (i=0;i<max_array;i++)
        queue[i] = 0;
    Bool done = FALSE;

    Coord current; init_coord(&current);
    Coord next; init_coord(&next);
    Coord coord; init_coord(&coord);
    Coord all_next[8];
    for (i=N;i<=NW;i++)
        init_coord(&all_next[i]);

    came_from[nstart] = nstart;
    queue[0] = nstart;
    cost[nstart] = 0;

    while (!done)
    {
        ncurrent = queue[0];
        if (ncurrent)
        {
            delete_from_queue(max_array, queue, ncurrent);
            current = conversion[ncurrent]; // get coord from number
            get_neighbours(all_next, current, max_coord);
            for (i=N;i<=NW;i++)
            {
                if (
                    is_pos_legal(all_next[i], start, max_coord, collision_map) &&
                    !are_corners_colliding(current, all_next[i], collision_map, max_coord)
                    )
                {
                    nnext = convert_coord_to_number(all_next[i], max_coord);
                    new_cost = calculate_cost(cost[ncurrent], all_next[i], cost_map);
                    if (a_star)
                        // Converts Dijkstra in A* : add cost f(distance)
                        new_cost += abs(goal.x - all_next[i].x) + abs(goal.y - all_next[i].y);

                    if (!came_from[nnext] || new_cost < cost[nnext])
                    {
                        came_from[nnext] = ncurrent;
                        cost[nnext] = new_cost;
                        set_in_queue(max_array, queue, nnext);
                    }
                    else
                        delete_from_queue(max_array, queue, ncurrent);

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
}

unsigned int find_path(
        Coord path[MAX_PATH_NODES],
        Coord const precise_start, Coord const precise_goal,
        Coord const max_coord,
        unsigned int** const collision_map,
        unsigned int** const cost_map
        )
{
    unsigned int nodes = 1, i,j,n;

    unsigned int x = precise_start.x, y = precise_start.y;
    Coord start; init_coord(&start);
    start.x = x; start.y = y;

    Coord goal; init_coord(&goal);
    x = precise_goal.x; y = precise_goal.y;
    goal.x = x; goal.y = y;

    unsigned int nstart = convert_coord_to_number(start, max_coord);
    unsigned int ngoal = convert_coord_to_number(goal, max_coord);
    unsigned int ncurrent = ngoal;
    unsigned int max_array = max_coord.x * max_coord.y;

    unsigned int cost[max_array+1], came_from[max_array+1];
    Coord conversion[max_array+1], coord;

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

    //A*
    dijkstra(TRUE, cost, came_from, conversion, start, goal, max_coord, collision_map, cost_map);

    path[0] = precise_goal;
    for (i=1;i<MAX_PATH_NODES;i++)
    {
        nodes++;
        path[i] = conversion[ncurrent];
        ncurrent = came_from[ncurrent]; // came_from[goal] = previous
        if (ncurrent == nstart)
        {
            if (i<MAX_PATH_NODES-1 && !is_same_coord(start, precise_start))
            {
                nodes++;
                path[i+1] = precise_start;
            }
            break;
        }
    }

    return nodes;
}
