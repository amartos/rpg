#ifndef RPG_MACROS
#define RPG_MACROS

// get var name
#define get_name(var)  #var

// common defs
#define TILES_WIDTH 32
#define TILES_HEIGHT 32
#define COLLISION_BOX_OFFSET_X 4
#define COLLISION_BOX_OFFSET_Y 28

// structures
typedef enum Bool Bool;
enum Bool
{
    FALSE = 0,
    TRUE = 1
};

#endif // define RPG_MACROS
