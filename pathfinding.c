#include "pathfinding.h"


static Bool is_in_queue(unsigned int const max_array, Coord queue[max_array], Coord const coord)
{
    unsigned int i;
    for (i=0;i<max_array;i++)
        if (is_same_coord(queue[i], coord))
            return TRUE;
    return FALSE;
}

static void set_in_queue(unsigned int const max_array, Coord queue[max_array], Coord const coord)
{
    unsigned int i;
    if (!is_in_queue(max_array, queue, coord))
        for (i=0;i<max_array;i++)
            if (is_coord_empty(queue[i]))
            {
                queue[i] = coord;
                break;
            }
}

static Bool is_queue_empty(unsigned int const max_array, Coord const queue[max_array])
{
    unsigned i;
    for (i=0;i<max_array;i++)
        if (!is_coord_empty(queue[i]))
            return FALSE;
    return TRUE;
}

static unsigned int get_max_score_queued(
        unsigned int const max_array,
        Coord const queue[max_array],
        unsigned int const maxx,
        unsigned int const cost[][maxx]
        )
{
    unsigned int i, x, y, max = 0;
    for (i=0;i<max_array;i++)
    {
        x = queue[i].x; y = queue[i].y;
        max = cost[y][x] < max ? max : cost[y][x];
    }
    return max;
}

static void count_scores_queued(
        unsigned int const max_array,
        Coord const queue[max_array],
        unsigned int const maxx,
        unsigned int const cost[][maxx],
        unsigned int const max_score,
        unsigned int counts[max_score+1]
        )
{
    unsigned int i, x, y;
    for (i=0;i<max_array;i++)
    {
        x = queue[i].x; y = queue[i].y;
        counts[cost[y][x]]++;
    }
}

static void copy_queue(
        unsigned int const max_array,
        Coord const from_queue[max_array],
        Coord to_queue[max_array]
        )
{
    unsigned int i;
    for (i=0;i<max_array;i++)
        to_queue[i] = from_queue[i];
}

static void order_queue(
        unsigned int const max_array,
        Coord queue[max_array],
        unsigned int const maxx,
        unsigned int const cost[][maxx]
        )
{
    unsigned int i, j, x, y, index = 0;
    unsigned int max = get_max_score_queued(max_array, queue, maxx, cost);
    unsigned int counts[max+1];
    for (i=0;i<=max;i++)
        counts[i] = 0;

    Coord new_queue[max_array];
    for (i=0;i<max_array;i++)
        init_coord(&new_queue[i]);

    count_scores_queued(max_array, queue, maxx, cost, max, counts);

    /* To sort the queue we set a loop from the lowest score (1) to the highest
     * score (max), and take out the coord in queue in order and put them in a
     * new queue. This new queue then replaces the former. */
    for (i=1;i<=max;i++) // score loop
        if (counts[i])
            for (j=0;j<max_array;j++) // queue loop
            {
                x = queue[j].x; y = queue[j].y;
                if (cost[y][x] == i)
                {
                    new_queue[index] = queue[j];
                    index++;
                    counts[i]--;
                    if (counts[i] <= 0)
                        break;
                }
            }

    copy_queue(max_array, new_queue, queue);
}

static void get_neighbours(Coord next[8], Coord const current, Coord const max_coord)
{
    /* 0 --------> max_coord.x - 1
     * | NW N NE
     * |  W . E
     * | SW S SE
     * v
     * max_coord.y - 1 */

    /* All conditionals are done in case the current coord correspond to one
     * of the edge of the map. */

    next[N].x = current.x;
    next[N].y = current.y > 0 ? current.y - 1 : current.y;

    next[NE].x = current.x < max_coord.x - 1 ? current.x + 1 : current.x;
    next[NE].y = current.y > 0 ? current.y - 1 : current.y;

    next[E].x = current.x < max_coord.x - 1 ?  current.x + 1 : current.x;
    next[E].y = current.y;

    next[SE].x = current.x < max_coord.x - 1 ? current.x + 1 : current.x;
    next[SE].y = current.y < max_coord.y - 1 ? current.y + 1 : current.y;

    next[S].x = current.x;
    next[S].y = current.y < max_coord.y - 1 ? current.y + 1 : current.y;

    next[SW].x = current.x > 0 ? current.x - 1 : current.x;
    next[SW].y = current.y < max_coord.y - 1 ? current.y + 1 : current.y;

    next[W].x = current.x > 0 ? current.x - 1 : current.x;
    next[W].y = current.y;

    next[NW].x = current.x > 0 ? current.x - 1 : current.x;
    next[NW].y = current.y > 0 ? current.y - 1 : current.y;
}

static unsigned int calculate_cost(
        unsigned int current_cost,
        Coord const next_coord,
        unsigned int** const cost_map,
        Bool const a_star,
        Coord const goal
        )
{
    unsigned int x = next_coord.x, y = next_coord.y;
    unsigned int distance = a_star ? abs(goal.x - next_coord.x) + abs(goal.y - next_coord.y) : 0;
    unsigned int next_cost = 1 + current_cost + cost_map[y][x] + distance;
    return next_cost;
}

static void pathfinding(
        Bool a_star,
        unsigned int const maxx,
        Coord came_from[][maxx],
        Coord start, Coord goal,
        Coord const max_coord,
        unsigned int** const collision_map,
        unsigned int** const cost_map
        )
{
    unsigned int i, x, y, new_cost;

    /* max_array is the maximum number of squares in the map. The worst case is
     * the queue being filled with each one of them.
     * This case should never happen in reality, but calculating the max number
     * of squares set in queue at worse is complicated. */
    unsigned int max_array = max_coord.x * max_coord.y;

    Coord queue[max_array];
    for (i=0;i<max_array;i++)
        init_coord(&queue[i]);

    Coord current; init_coord(&current);

    Coord next[NW+1];
    for (i=N;i<=NW;i++)
        init_coord(&next[i]);
    unsigned int nextx = 0, nexty = 0;

    unsigned int maxy = max_coord.y, cost[maxy][maxx];
    for (y=0;y<maxy;y++)
        for (x=0;x<maxx;x++)
            cost[y][x] = 0;

    unsigned int currx = start.x, curry = start.y;
    /* Here we set "start came from start" to avoid the algorithm to analyse the
     * start square when the current position is a neighbour. */
    came_from[curry][currx] = start;
    cost[curry][currx] = 0;
    queue[0] = start;

    // Here is the pathfinding loop
    while (TRUE)
    {
        current = queue[0];
        reset_coord(&queue[0]);

        if (is_same_coord(goal, current))
            break;
        else if (!is_coord_empty(current))
        {
            get_neighbours(next, current, max_coord);

            for (i=N;i<=NW;i++)
                if (
                    is_pos_legal(next[i], start, max_coord, collision_map) &&
                    !are_corners_colliding(current, next[i], collision_map, max_coord)
                    )
                {
                    nextx = next[i].x; nexty = next[i].y;
                    new_cost = calculate_cost(cost[curry][currx], next[i], cost_map, a_star, goal);
                    if (is_coord_empty(came_from[nexty][nextx]) || new_cost < cost[nexty][nextx])
                    {
                        came_from[nexty][nextx] = current;
                        cost[nexty][nextx] = new_cost;
                        set_in_queue(max_array, queue, next[i]);
                    }
                }
        }
        else if (is_queue_empty(max_array, queue))
            break;

        order_queue(max_array, queue, maxx, cost);
    }
}

static void dijkstra(
        unsigned int const maxx,
        Coord came_from[][maxx],
        Coord start, Coord goal,
        Coord const max_coord,
        unsigned int** const collision_map,
        unsigned int** const cost_map
        )
{
    pathfinding(FALSE, maxx, came_from, start, goal, max_coord, collision_map, cost_map);
}

static void a_star(
        unsigned int const maxx,
        Coord came_from[][maxx],
        Coord start, Coord goal,
        Coord const max_coord,
        unsigned int** const collision_map,
        unsigned int** const cost_map
        )
{
    pathfinding(TRUE, maxx, came_from, start, goal, max_coord, collision_map, cost_map);
}

unsigned int find_path(
        Coord path[MAX_PATH_NODES],
        Coord const precise_start, Coord const precise_goal,
        Coord const max_coord,
        unsigned int** const collision_map,
        unsigned int** const cost_map
        )
{
    unsigned int i, x, y, nodes = 1;

    Coord start = click_to_tile(precise_start);
    Coord goal = click_to_tile(precise_goal);

    unsigned int maxx = max_coord.x, maxy = max_coord.y;
    Coord came_from[maxy][maxx];
    for (y=0;y<maxy;y++)
        for (x=0;x<maxx;x++)
            init_coord(&came_from[y][x]);

    a_star(maxx, came_from, start, goal, max_coord, collision_map, cost_map);

    path[0] = precise_goal;
    Coord current = goal;
    for (i=1;i<MAX_PATH_NODES;i++)
    {
        nodes++;
        path[i] = current;
        x = current.x; y = current.y;
        current = came_from[y][x]; // came_from[goal] = previous
        if (is_same_coord(current, start))
        {
            if (i<MAX_PATH_NODES-1 && !is_same_coord(start, precise_start))
            {
                nodes++;
                path[i+1] = precise_start;
            }
            break;
        }
    }

    /* if nodes did not increase, it means that no path was found to the goal.
     * Thus, set it to 0 to reflect this. */
    nodes = nodes > 1 ? nodes : 0;

    return nodes;
}
