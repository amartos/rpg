#include "movement.h"


static void teleport(Movement *movement)
{
    movement->position = movement->path[movement->current_node];
    reset_coord(&movement->path[movement->current_node]);
}

void init_movement(Movement *movement)
{
    unsigned int i;
    init_coord(&movement->position);
    init_coord(&movement->formation_offset);
    for (i=0;i<MAX_PATH_NODES;i++)
        init_coord(&movement->path[i]);
    movement->moving = FALSE;
    movement->velocity = 0;
    movement->direction = S;
    movement->current_node = 0;
    movement->movement_type = WALK;
    movement->formation = LINE;
}

/* This function exists because in cases the velocity is not a multiple of the
 * remaining distance, the character will enter in a movement loop and never
 * reaches the goal. Thus, this function returns the remaining difference
 * between the velocity and the goal's distance, which is then used to
 * "decrease" the velocity by such. */
static Coord determine_decrease(Movement const movement)
{
    Coord decrease; init_coord(&decrease);
    Coord goal; init_coord(&goal);

    // calculate the remaining distance to goal.
    goal.x = fabs(movement.position.x - movement.path[movement.current_node].x);
    goal.y = fabs(movement.position.y - movement.path[movement.current_node].y);

    decrease.x = fmod(goal.x, movement.velocity);
    decrease.y = fmod(goal.y, movement.velocity);

    return decrease;
}

static void walk(Movement *movement)
{
    movement->direction = determine_direction(
            movement->position,
            movement->path[movement->current_node]
            );
    Coord decrease = determine_decrease(*movement);

    // We make two switches here to handle either x or y coord changes
    switch(movement->direction)
    {
        case N:
        case NE:
        case NW:
            movement->position.y -= movement->velocity - decrease.y;
            break;
        case S:
        case SE:
        case SW:
            movement->position.y += movement->velocity - decrease.y;
            break;
        default:
            break;
    }

    switch(movement->direction)
    {
        case E:
        case NE:
        case SE:
            movement->position.x += movement->velocity - decrease.x;
            break;
        case W:
        case SW:
        case NW:
            movement->position.x -= movement->velocity - decrease.x;
            break;
        default:
            break;
    }
}

/* This function is to handle a special rounding.
 * If n is-0.5 <= n <= 0.5, round to 0.0.
 * If abs(n) > 0.5, round to the highest.
 * Then returns the rounded number, *-1 if n < 0. */
static double round_angle(float n)
{
    double x = fabs(n) <= 0.5 ? 0.0 : ceil(fabs(n));
    return n < 0 ? -1*x : x;
}

// sin(x) takes x as radians, so sind(x) takes x as degrees
static double sind(unsigned int angle)
{
    return sin(radians(angle));
}

// cos(x) takes x as radians, so cosd(x) takes x as degrees
static double cosd(unsigned int angle)
{
    return cos(radians(angle));
}

/* When the formation changes, makes the characters move to adopt the new
 * formation. */
void change_formation(Movement *movement, Deployment formation)
{
    movement->formation = formation;
    fire_movement(movement, PATH);
}

/* This function calculates the offset of each character other than main
 * (char_number == 0) depending on the formation they are actually in and stores
 * the offset in the Movement struct. */
static void calc_formation_offset(Movement *movement, unsigned int const char_number)
{
    /* most of this funtion will depend on the MAX_CHARACTERS, but cannot be
     * linked as it is very specific, thus need to be independently defined
     * TODO: change this. */

    double ox = 0, oy = 0;
    double half_cn = char_number/2, third_cn = char_number/3;
    Cardinals direction = movement->direction;

    // do not change main character position, used as it is used as reference
    if (char_number > 0)
    {
        /* This calculates the position of each character depending on the
         * formation set. The comments show a general scheme of the formation
         * with the character numbers (0 as main), and in the movement
         * direction left-right (->). */
        switch(movement->formation)
        {
            // Line: 3210
            case LINE:
                ox = round_angle(cosd(270 + 45*direction)) * (-1 * char_number);
                oy = round_angle(sind(270 + 45*direction)) * (-1 * char_number);
                break;
            /* square:
             * 01
             * 23 */
            case SQUARE:
                ox = round_angle(cosd(270 + 45*char_number + 45*direction));
                oy = round_angle(sind(270 + 45*char_number + 45*direction));
                break;
            /* "triangle":
             * 2
             *  01
             * 3 */
            case TRIANGLE:
                ox = round_angle(cosd(270 - half_cn * 135 - third_cn * 90 + 45*direction));
                oy = round_angle(sind(270 - half_cn * 135 - third_cn * 90 + 45*direction));
                break;
            /* circle (this is actually a half circle):
             * 0
             *  1
             *  2
             * 3 */
            case CIRCLE:
                ox = round_angle(cosd(270 + 45*char_number + 45*direction)) + 1 + third_cn;
                oy = round_angle(sind(270 + 45*char_number + 45*direction));
                break;
            default:
                break;
        }

        movement->formation_offset.x = ox;
        movement->formation_offset.y = oy;
    }
}

void deploy(Coord *position, Movement *movement, unsigned int const char_number)
{
    calc_formation_offset(movement, char_number);
    *position = sum_coord(*position, movement->formation_offset);
}

void fire_movement(Movement *movement, MovementType const movement_type)
{
    movement->moving = TRUE;
    movement->movement_type = movement_type;
}

void stop_movement(Movement *movement)
{
    movement->current_node = 0;
    reset_coord(&movement->path[0]);
    movement->movement_type = WALK;
    movement->moving = FALSE;
}

void move(
        Movement *movement,
        Coord const max_coord,
        unsigned int** const collision_map,
        unsigned int** const cost_map
        )
{
    switch (movement->movement_type)
    {
        case PATH: // triggers pathfinding
            movement->movement_type = WALK;
            unsigned int nodes = find_path(
                    movement->path,
                    movement->position,
                    movement->path[movement->current_node],
                    max_coord,
                    collision_map,
                    cost_map
                    );
            if (nodes)
            {
                /* find_path returns the total number of nodes, but current_node
                 * is the index of the movement->path array, thus the -1 */
                movement->current_node = nodes - 1;
                goto walking;
            }
            else
                goto end_move; // no nodes = no path found
        case WALK:
            goto walking;
        case TELEPORT:
            teleport(movement);
            goto end_move;
        walking:
            walk(movement);
            Coord position = movement->position;
            Coord current_goal = movement->path[movement->current_node];
            if (is_same_coord(position, current_goal))
            {
                if (movement->current_node <= 0)
                    goto end_move;
                else
                    movement->current_node--;
            }
            break;
        end_move:
            stop_movement(movement);
            break;
    }
}
