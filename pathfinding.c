#include "pathfinding.h"


static void set_in_queue(unsigned int const max_array, Coord queue[max_array], Coord const next)
{
    unsigned int i;
    Bool already_in = FALSE;
    for (i=0;i<max_array;i++)
        if (is_same_coord(queue[i], next))
            already_in = TRUE;
    if (!already_in)
        for (i=0;i<max_array;i++)
            if (is_coord_empty(queue[i]))
            {
                queue[i] = next;
                break;
            }
}

static void delete_from_queue(unsigned int const max_array, Coord queue[max_array], Coord const coord)
{
    unsigned int i;
    for (i=0;i<max_array;i++)
        if (is_same_coord(queue[i], coord))
            reset_coord(&queue[i]);
}

static Bool check_queue(unsigned int const max_array, Coord const queue[max_array])
{
    unsigned i;
    Bool done = TRUE;
    for (i=0;i<max_array;i++)
        if (!is_coord_empty(queue[i]))
            done = FALSE;
    return done;
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

    // find lowest scores in order in queue and sets them first in new_queue
    // score 0 used as NULL thus skipped
    // scores not in queue also skipped
    for (i=1;i<=max;i++) // score
        if (counts[i])
            for (j=0;j<max_array;j++) // order of queue
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
        unsigned int const maxx,
        unsigned int cost[][maxx],
        Coord came_from[][maxx],
        Coord start, Coord goal,
        Coord const max_coord,
        unsigned int** const collision_map,
        unsigned int** const cost_map
        )
{
    unsigned int i = 0, new_cost = 0;
    unsigned int max_array = max_coord.x * max_coord.y;
    unsigned int currx = start.x, curry = start.y;
    unsigned int nextx = 0, nexty = 0;
    Coord queue[max_array];
    for (i=0;i<max_array;i++)
        init_coord(&queue[i]);
    Bool done = FALSE;

    Coord current; init_coord(&current);
    Coord next[8];
    Coord coord; init_coord(&coord);

    for (i=N;i<=NW;i++)
        init_coord(&next[i]);

    came_from[curry][currx] = start; // start came from start
    queue[0] = start;
    cost[curry][currx] = 0; // cost start

    while (!done)
    {
        current = queue[0];
        if (!is_coord_empty(current))
        {
            delete_from_queue(max_array, queue, current);
            get_neighbours(next, current, max_coord);
            for (i=N;i<=NW;i++)
            {
                if (
                    is_pos_legal(next[i], start, max_coord, collision_map) &&
                    !are_corners_colliding(current, next[i], collision_map, max_coord)
                    )
                {
                    nextx = next[i].x; nexty = next[i].y;
                    new_cost = calculate_cost(cost[curry][currx], next[i], cost_map);
                    if (a_star)
                        // Converts Dijkstra in A* : add cost f(distance)
                        new_cost += abs(goal.x - next[i].x) + abs(goal.y - next[i].y);

                    if (is_coord_empty(came_from[nexty][nextx]) || new_cost < cost[nexty][nextx])
                    {
                        came_from[nexty][nextx] = current;
                        cost[nexty][nextx] = new_cost;
                        set_in_queue(max_array, queue, next[i]);
                    }
                    else
                        delete_from_queue(max_array, queue, current);

                    if (is_same_coord(next[i], goal))
                    {
                        done = TRUE;
                        break;
                    }
                }
            }

            if (!done)
            {
                order_queue(max_array, queue, maxx, cost);
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
    unsigned int nodes = 1, i;

    unsigned int x = precise_start.x, y = precise_start.y;
    Coord start = int_to_coord(x, y);
    x = precise_goal.x; y = precise_goal.y;
    Coord goal = int_to_coord(x, y);

    unsigned int maxx = max_coord.x, maxy = max_coord.y;

    unsigned int cost[maxy][maxx];
    Coord came_from[maxy][maxx];

    for (y=0;y<maxy;y++)
        for (x=0;x<maxx;x++)
        {
            cost[y][x] = 0;
            init_coord(&came_from[y][x]);
        }

    //A*
    dijkstra(TRUE, maxx, cost, came_from, start, goal, max_coord, collision_map, cost_map);

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

    return nodes;
}
