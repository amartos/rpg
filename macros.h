#ifndef RPG_MACROS
#define RPG_MACROS

#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>


// ------------------------------------
// Coding stuff
// ------------------------------------

// get var name
#define get_name(var)  #var
#define MAX_SIZE_LINE 1000

typedef enum Bool Bool;
enum Bool
{
    FALSE = 0,
    TRUE = 1
};

// ------------------------------------
// Images
// ------------------------------------

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 640
#define SCREEN_BPP 32
#define FPS 60
#define FRAMERATE 1000/FPS
#define COLOR_PALETTE 3
#define BG_COLOR 0xFF, 0xFF, 0xFF

typedef enum ColorChannels ColorChannels;
enum ColorChannels {R, G, B};

typedef enum ColorPalette ColorPalette;
enum ColorPalette 
{
    GREEN,
    RED,
    BLUE,
    YELLOW
};

// This is an embryo of a tile structure, unused yet
typedef struct Tile Tile;
struct Tile
{
    SDL_Surface *image;
    int collision;
};

// ------------------------------------
// Movement
// ------------------------------------

#define MAX_PATH_NODES 100

// this order depends on the sprite order
typedef enum Cardinals Cardinals;
enum Cardinals {W, S, N, E, NE, NW, SE, SW};

typedef enum Deployment Deployment;
enum Deployment {
    LINE,
    SQUARE,
    TRIANGLE,
    CIRCLE
};

typedef struct Coord Coord;
struct Coord
{
    unsigned int x;
    unsigned int y;
    int ox; // offset
    int oy;
};

typedef enum MovementType MovementType;
enum MovementType {WALK, PATH, TELEPORT};

// ------------------------------------
// Characters
// ------------------------------------

#define SPRITES_WIDTH 32
#define SPRITES_HEIGHT 32
#define COLLISION_BOX_OFFSET_X 4
#define COLLISION_BOX_OFFSET_Y 28
#define COLLISION_BOX_WIDTH (SPRITES_WIDTH - 2 * COLLISION_BOX_OFFSET_X) // left & right
#define COLLISION_BOX_HEIGHT (SPRITES_HEIGHT - COLLISION_BOX_OFFSET_Y)
#define MAX_CHARACTERS 4

typedef enum State State;
enum State 
{
    MOVE,
    LIFT,
    PUSH,
    PULL,
    PUSH_SHIELD,
    MOVE_SHIELD,
    STILL
};

typedef struct OnScreen OnScreen;
struct OnScreen 
{
    SDL_Surface *sprite;
    State state;
    Bool animated;
    // possible positions, State, max number of frames
    SDL_Rect frames[SW+1][MOVE_SHIELD+1][FPS];
    unsigned int total_frames; // total
    unsigned int current_frame;
    unsigned int framerate;
    unsigned int time; // previous tick (for changing frame)
};

typedef struct Movement Movement;
struct Movement 
{
    Coord position; // current position and offset
    Bool moving;
    Cardinals direction;
    Coord path[MAX_PATH_NODES]; // array of nodes
    unsigned int current_node;
    MovementType movement_type;
    unsigned int velocity; // in pixels
};

// This describes a Character type object, aka all what can be described as a
// living being
typedef struct Character Character;
struct Character
{
    unsigned int id;
    OnScreen on_screen;
    Movement movement;
};

// ------------------------------------
// Maps
// ------------------------------------

#define TILES_WIDTH 32
#define TILES_HEIGHT 32

// Background always fist, weather always last
typedef enum MapType MapType;
enum MapType {
    BACKGROUND,
    FOREGROUND,
    COLLISIONS,
    COST,
    WEATHER
};

typedef struct Map Map;
struct Map
{
    unsigned int w, h;
    unsigned int x_tiles, y_tiles;
    unsigned int total_tiles;
    unsigned int xscroll, yscroll;
    unsigned int ***schematics;
};

#endif // define RPG_MACROS
