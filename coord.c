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

Coord int_to_coord(unsigned int const x, unsigned int const y)
{
    Coord coord; init_coord(&coord);
    coord.x = x; coord.y = y;
    return coord;
}

Coord isometric_to_cartesian(Coord const isometric, Camera const camera)
{
    Coord cartesian; init_coord(&cartesian);
    cartesian.x = isometric.y/TILES_HEIGHT + (isometric.x - SCREEN_WIDTH/2)/TILES_WIDTH;
    cartesian.y = isometric.y/TILES_HEIGHT - (isometric.x - SCREEN_WIDTH/2)/TILES_WIDTH;
    return cartesian;
}

Coord cartesian_to_isometric(Coord const cartesian, Camera const camera)
{
    Coord isometric; init_coord(&isometric);
    isometric.x = (cartesian.x - cartesian.y) * TILES_WIDTH/2 + SCREEN_WIDTH/2;
    isometric.y = (cartesian.x + cartesian.y) * TILES_HEIGHT/2;
    return isometric;
}

Coord event_to_coord(Sint32 x, Sint32 y, Camera const camera)
{
    Coord map; init_coord(&map);
    map.x = x; map.y = y;
    map = isometric_to_cartesian(map, camera);
    map.x += camera.scroll.x; map.y += camera.scroll.y; // scroll correction
    return map;
}

SDL_Rect coord_to_isosdlrect(Coord const coord, Camera const camera)
{

    Coord isometrified = coord;
    isometrified.x -= camera.scroll.x; isometrified.y -= camera.scroll.y; // scroll correction
    isometrified = cartesian_to_isometric(isometrified, camera);
    SDL_Rect rect;
    rect.x = isometrified.x; rect.y = isometrified.y;
    rect.w = TILES_WIDTH; rect.h = TILES_HEIGHT;
    return rect;
}

Bool is_same_coord(Coord const a, Coord const b)
{
    Bool same = FALSE;
    if (
            is_same_double(a.x, b.x) &&
            is_same_double(a.y, b.y)
       )
        same = TRUE;
    return same;
}

Bool is_within_tile(Coord const a, Coord const b)
{
    Bool within = FALSE;
    unsigned int ax = a.x, ay = a.y;
    unsigned int bx = b.x, by = b.y;
    if (ax == bx && ay == by)
        within = TRUE;
    return within;
}

Bool is_colliding(Coord const goal, unsigned int** const collision_map, Coord const max_coord)
{
    unsigned int x = goal.x, y = goal.y;
    if(!is_out_of_map(goal, max_coord))
        return collision_map[y][x];
    return TRUE;
}

Bool are_corners_colliding(Coord const start, Coord const goal, unsigned int** const collision_map, Coord const max_coord)
{
    Coord edge1; init_coord(&edge1);
    Coord edge2; init_coord(&edge2);
    Cardinals direction = determine_direction(start, goal);
    Bool collide_edge1 = FALSE;
    Bool collide_edge2 = FALSE;

    switch (direction)
    {
        case NW:
            // North
            edge1.x = start.x;
            edge1.y = start.y - 1;

            // West
            edge2.x = start.x - 1;
            edge2.y = start.y;
            break;
        case NE:
            // North
            edge1.x = start.x;
            edge1.y = start.y - 1;

            // East
            edge2.x = start.x + 1;
            edge2.y = start.y;
            break;
        case SW:
            // South
            edge1.x = start.x;
            edge1.y = start.y + 1;

            // West
            edge2.x = start.x - 1;
            edge2.y = start.y;
            break;
        case SE:
            // North
            edge1.x = start.x;
            edge1.y = start.y + 1;

            // East
            edge2.x = start.x + 1;
            edge2.y = start.y;
            break;
        default:
            return FALSE;
    }

    collide_edge1 = is_colliding(edge1, collision_map, max_coord);
    collide_edge2 = is_colliding(edge2, collision_map, max_coord);
    if (collide_edge1 || collide_edge2)
        return TRUE;
    else
        return FALSE;
}

Bool is_out_of_map(Coord const goal, Coord const max_coord)
{
    Bool is_out = FALSE;
    if (goal.x < 0 || goal.y < 0 || goal.x >= max_coord.x || goal.y >= max_coord.y)
        is_out = TRUE;

    return is_out;
}

Bool is_pos_legal(
        Coord const position,
        Coord const char_pos,
        Coord const max_coord,
        unsigned int** const collisions
        )
{
    if (
            !is_same_coord(position, char_pos) &&
            !is_out_of_map(position, max_coord) &&
            !is_colliding(position, collisions, max_coord)
        )
        return TRUE;
    else
        return FALSE;
}

Cardinals determine_direction(Coord const start, Coord const goal)
{
    double angle = atan2(goal.y - start.y, goal.x - start.x)*180/M_PI;
    int direction = round((90+angle) / 45);
    // if -135°, it corresponds to the 7th value, but gives -1
    if (direction < 0)
        direction = 7;
    return direction;
}

void init_camera(Camera *camera)
{
    init_coord(&camera->scroll);
    camera->scale = 1;
}

double scale(unsigned int const size, Camera const camera)
{
    double scaled = size * camera.scale;
    return scaled;
}
