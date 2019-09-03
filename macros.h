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

typedef enum Cardinals Cardinals;
// this order depends on the sprite order
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
    MOVE_SHIELD
};

typedef struct Character Character;
struct Character
{
    SDL_Surface *sprite;
    Coord position;
    Cardinals direction;
    Coord goal;
    Coord *path;
    unsigned int nodes;
    State state;
    Bool animated;
    MovementType movement_type;
    Bool moving;
    unsigned int number_of_frames;
    unsigned int velocity;
    SDL_Rect frames[4][6][30]; // possible positions, State, max number of frames
    unsigned int current_frame;
    unsigned int framerate;
    unsigned int previous_time;
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
