#include "coord.h"


void init_coord(Coord *coord)
{
    coord->x = 0.0;
    coord->y = 0.0;
}

void reset_coord(Coord *coord)
{
    init_coord(coord);
}

Coord isometric_to_cartesian(Coord const isometric)
{
    Coord cartesian; init_coord(&cartesian);
    cartesian.x = isometric.y/TILES_HEIGHT + (isometric.x - SCREEN_WIDTH/2)/TILES_WIDTH;
    cartesian.y = isometric.y/TILES_HEIGHT - (isometric.x - SCREEN_WIDTH/2)/TILES_WIDTH;
    return cartesian;
}

Coord cartesian_to_isometric(Coord const cartesian)
{
    Coord isometric; init_coord(&isometric);
    isometric.x = (cartesian.x - cartesian.y) * TILES_WIDTH/2 + SCREEN_WIDTH/2;
    isometric.y = (cartesian.x + cartesian.y) * TILES_HEIGHT/2;
    return isometric;
}

SDL_Rect coord_to_isosdlrect(Coord const coord)
{
    Coord isometrified = cartesian_to_isometric(coord);
    SDL_Rect rect;
    rect.x = isometrified.x; rect.y = isometrified.y;
    rect.w = TILES_WIDTH; rect.h = TILES_HEIGHT;
    return rect;
}

Bool is_same_coord(Coord const a, Coord const b)
{
    Bool same = FALSE;
    if (a.x == b.x && a.y == b.y)
        same = TRUE;
    return same;
}

Bool is_colliding(Coord const goal, unsigned int** const collision_map)
{
    unsigned int x = goal.x, y = goal.y;
    return collision_map[x][y];
}

Bool are_corners_colliding(Coord const start, Coord const goal, unsigned int** const collision_map)
{
    Coord edge_coord; init_coord(&edge_coord);
    Cardinals direction = determine_direction(start, goal);
    Bool edge1 = FALSE, edge2 = FALSE;

    switch (direction)
    {
        case NW:
            // North
            edge_coord.x = start.x;
            edge_coord.y = start.y - 1;
            edge1 = is_colliding(edge_coord, collision_map);

            // West
            edge_coord.x = start.x - 1;
            edge_coord.y = start.y;
            edge2 = is_colliding(edge_coord, collision_map);
            break;
        case NE:
            // North
            edge_coord.x = start.x;
            edge_coord.y = start.y - 1;
            edge1 = is_colliding(edge_coord, collision_map);

            // East
            edge_coord.x = start.x + 1;
            edge_coord.y = start.y;
            edge2 = is_colliding(edge_coord, collision_map);
            break;
        case SW:
            // South
            edge_coord.x = start.x;
            edge_coord.y = start.y + 1;
            edge1 = is_colliding(edge_coord, collision_map);

            // West
            edge_coord.x = start.x - 1;
            edge_coord.y = start.y;
            edge2 = is_colliding(edge_coord, collision_map);
            break;
        case SE:
            // North
            edge_coord.x = start.x;
            edge_coord.y = start.y + 1;
            edge1 = is_colliding(edge_coord, collision_map);

            // East
            edge_coord.x = start.x + 1;
            edge_coord.y = start.y;
            edge2 = is_colliding(edge_coord, collision_map);
            break;
        default:
            return FALSE;
    }

    if (edge1 || edge2)
        return TRUE;
    else
        return FALSE;
}

Bool is_out_of_map(Coord const goal, Coord const max_coord)
{
    Bool is_out = FALSE;
    int x = goal.x, y = goal.y;
    int mx = max_coord.x, my = max_coord.y;

    if (x < 0 || y < 0 || x >= mx || y >= my)
        is_out = TRUE;

    return is_out;
}

Cardinals determine_direction(Coord const start, Coord const goal)
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
