#ifndef RPG_MACROS
#define RPG_MACROS

// get var name
#define get_name(var)  #var

// common defs
#define TILES_WIDTH 32
#define TILES_HEIGHT 32
#define SPRITES_WIDTH 32
#define SPRITES_HEIGHT 32
#define COLLISION_BOX_OFFSET_X 4
#define COLLISION_BOX_OFFSET_Y 28
#define COLLISION_BOX_WIDTH (SPRITES_WIDTH - 2 * COLLISION_BOX_OFFSET_X) // left & right
#define COLLISION_BOX_HEIGHT (SPRITES_HEIGHT - COLLISION_BOX_OFFSET_Y)
#define MAX_CHARACTERS 4


// structures
typedef enum Bool Bool;
enum Bool
{
    FALSE = 0,
    TRUE = 1
};

#endif // define RPG_MACROS
