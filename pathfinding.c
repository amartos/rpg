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

/* We use this function to merge the temp queue within the main queue
 * at start_index of main queue. */
static void merge_queue(
        unsigned int start_index,
        unsigned int stop_index,
        Coord temp_queue[],
        Coord main_queue[]
        )
{
    unsigned int i, temp_index;
    for (i=start_index;i<stop_index;i++)
    {
        /* The temp queue starts at 0, thus the start_index is removed from the loop
         * index var to obtain its own index. */
        temp_index = i - start_index;
        if (is_coord_empty(temp_queue[temp_index]))
            break;
        main_queue[i] = temp_queue[temp_index];
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

// This is the implementation of the quick sorting algorithm for the queues.
static void quick_sort(
        unsigned int const max_array,
        Coord queue[max_array],
        unsigned int const maxx,
        unsigned int const cost[][maxx]
        )
{
    // In case the array is of size 1, just stops the recursion.
    if (max_array <= 1)
        return;

    unsigned int i, x, y;

    /* Here is the pivot definition. Given the pathfinding algorithm, the pivot
     * has a lot of chances to be a set of NULL coords, with a score of 0. */
    unsigned int ipivot = max_array/2;
    x = queue[ipivot].x; y = queue[ipivot].y;
    unsigned int pivot_cost = cost[y][x];

    /* These are the 3 parts that we are interested in. On the "left" side of
     * the queue we want the costs lower than the pivot's, the center are the
     * same costs as the pivot, and the right are the bigger costs than the
     * pivots.
     * All empty coords are ignored, but NOT coords with a cost of 0. */
    unsigned int ileft = 0, icenter = 0, iright = 0;
    Coord left[max_array], center[max_array], right[max_array];
    for (i=0;i<max_array;i++)
    {
        init_coord(&left[i]);
        init_coord(&center[i]);
        init_coord(&right[i]);
    }

    /* This is a temporary queue we fill with the ordered coords, and then use
     * it to replace the queue. */
    Coord ordered_queue[max_array];
    for (i=0;i<max_array;i++)
        init_coord(&ordered_queue[i]);

    // This is the main loop of the algo, which analyses the queue.
    for (i=0;i<max_array;i++)
    {
        if (is_coord_empty(queue[i]))
            continue;

        x = queue[i].x; y = queue[i].y;
        if (cost[y][x] < pivot_cost)
        {
            left[ileft] = queue[i];
            ileft++;
        }
        else if (cost[y][x] == pivot_cost)
        {
            center[icenter] = queue[i];
            icenter++;
        }
        else if (cost[y][x] > pivot_cost)
        {
            right[iright] = queue[i];
            iright++;
        }
    }

    /* This is the recursion part.
     * The center is ignored, as all the costs scores are the same, == to the
     * pivot cost. */
    quick_sort(ileft, left, maxx, cost);
    quick_sort(iright, right, maxx, cost);

    /* Here we merge the 3 arrays, left, center and right, in the ordered_queue
     * array. Empty coords are ignored too. */
    merge_queue(0, ileft, left, ordered_queue);
    merge_queue(ileft, ileft+icenter, center, ordered_queue);
    merge_queue(ileft+icenter, ileft+icenter+iright, right, ordered_queue);

    // We replace the queue by the ordered queue.
    copy_queue(max_array, ordered_queue, queue);
}

static void get_neighbours(Coord next[8], Coord const current, Map const map)
{
    /* 0 --------> map.max.x - 1
     * | NW N NE
     * |  W . E
     * | SW S SE
     * v
     * map.max.y - 1 */

    /* All conditionals are done in case the current coord correspond to one
     * of the edge of the map. */

    next[N].x = current.x;
    next[N].y = current.y > 0 ? current.y - 1 : current.y;

    next[NE].x = current.x < map.max.x - 1 ? current.x + 1 : current.x;
    next[NE].y = current.y > 0 ? current.y - 1 : current.y;

    next[E].x = current.x < map.max.x - 1 ?  current.x + 1 : current.x;
    next[E].y = current.y;

    next[SE].x = current.x < map.max.x - 1 ? current.x + 1 : current.x;
    next[SE].y = current.y < map.max.y - 1 ? current.y + 1 : current.y;

    next[S].x = current.x;
    next[S].y = current.y < map.max.y - 1 ? current.y + 1 : current.y;

    next[SW].x = current.x > 0 ? current.x - 1 : current.x;
    next[SW].y = current.y < map.max.y - 1 ? current.y + 1 : current.y;

    next[W].x = current.x > 0 ? current.x - 1 : current.x;
    next[W].y = current.y;

    next[NW].x = current.x > 0 ? current.x - 1 : current.x;
    next[NW].y = current.y > 0 ? current.y - 1 : current.y;
}

static unsigned int calculate_cost(
        unsigned int current_cost,
        Coord const next_coord,
        Map const map,
        Bool const a_star,
        Coord const goal
        )
{
    unsigned int x = next_coord.x, y = next_coord.y;
    unsigned int distance = a_star ? abs(goal.x - next_coord.x) + abs(goal.y - next_coord.y) : 0;
    unsigned int next_cost = 1 + current_cost + map.cost[y][x] + distance;
    return next_cost;
}

static void pathfinding(
        Bool a_star,
        unsigned int const maxx,
        Coord came_from[][maxx],
        Coord start, Coord goal,
        Map const map
        )
{
    unsigned int i, x, y, new_cost;

    /* max_array is the maximum number of squares in the map divide by 10.
     * The worst case is the queue being filled with each one of them. In
     * practice, the queue is never filled with more than 300ish for a map of
     * more than 13k tiles. Division by 10 keeps a security margin, with a
     * significant increase in speed. */
    unsigned int max_array = map.max.x * map.max.y/10;

    Coord queue[max_array];
    for (i=0;i<max_array;i++)
        init_coord(&queue[i]);

    Coord current; init_coord(&current);

    Coord next[NW+1];
    for (i=N;i<=NW;i++)
        init_coord(&next[i]);
    unsigned int nextx = 0, nexty = 0;

    unsigned int maxy = map.max.y, cost[maxy][maxx];
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
            get_neighbours(next, current, map);

            for (i=N;i<=NW;i++)
                if (
                    is_pos_legal(next[i], start, map) &&
                    !are_corners_colliding(current, next[i], map)
                    )
                {
                    nextx = next[i].x; nexty = next[i].y;
                    new_cost = calculate_cost(cost[curry][currx], next[i], map, a_star, goal);
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

        quick_sort(max_array, queue, maxx, cost);
    }
}

static void dijkstra(
        unsigned int const maxx,
        Coord came_from[][maxx],
        Coord start, Coord goal,
        Map const map
        )
{
    pathfinding(FALSE, maxx, came_from, start, goal, map);
}

static void a_star(
        unsigned int const maxx,
        Coord came_from[][maxx],
        Coord start, Coord goal,
        Map const map
        )
{
    pathfinding(TRUE, maxx, came_from, start, goal, map);
}

unsigned int find_path(
        Coord path[MAX_PATH_NODES],
        Coord const precise_start, Coord const precise_goal,
        Map const map
        )
{
    unsigned int i, x, y, nodes = 1;

    Coord start = click_to_tile(precise_start);
    Coord goal = click_to_tile(precise_goal);

    unsigned int maxx = map.max.x, maxy = map.max.y;
    Coord came_from[maxy][maxx];
    for (y=0;y<maxy;y++)
        for (x=0;x<maxx;x++)
            init_coord(&came_from[y][x]);

    a_star(maxx, came_from, start, goal, map);

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
