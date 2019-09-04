#include "coord.h"


void init_coord(Coord *coord)
{
    coord->x = 0;
    coord->y = 0;
}

void reset_coord(Coord *coord)
{
    init_coord(coord);
}

void unit_to_pixels(Coord *coord)
{
    coord->x *= TILES_WIDTH;
    coord->y *= TILES_HEIGHT;
}

void pixels_to_unit(Coord *coord)
{
    coord->x /= TILES_WIDTH;
    coord->y /= TILES_HEIGHT;
}

void round_coord(Coord *coord)
{
    // This operation, apparently doing nothing, in fact round the coordinates
    // up to the corresponding tile. The coord is given in pixels.
    pixels_to_unit(coord);
    unit_to_pixels(coord);
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
    printf("(%d, %d), (%d, %d), ", xl, yu, xr, yd);

    printf("(%d, %d), ", collision_map[xl][yu], collision_map[xr][yd]);
    if (collision_map[xl][yu] || collision_map[xr][yd]) // up left || down right corners
    {
        printf("1\n");
        return TRUE;
    }
    else
    {
        printf("0\n");
        return FALSE;
    }
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
