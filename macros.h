#ifndef RPG_MACROS
#define RPG_MACROS

#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>


// ------------------------------------
// Coding stuff
// ------------------------------------

#define get_name(var)  #var // get var name
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

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 640
#define SCREEN_BPP 32
#define FPS 120
#define FRAMERATE 1000/FPS
#define COLOR_PALETTE 3
#define BG_COLOR 0xFF, 0xFF, 0xFF

typedef enum ColorChannels ColorChannels;
enum ColorChannels {R, G, B};

// This is the struct used to define the game color palette to be applied to
// greys sprites
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
// Characters
// ------------------------------------

// This describes the character's image on screen
#define SPRITES_WIDTH 32
#define SPRITES_HEIGHT 32
#define COLLISION_BOX_OFFSET_X 4
#define COLLISION_BOX_OFFSET_Y 28
#define COLLISION_BOX_WIDTH (SPRITES_WIDTH - 2 * COLLISION_BOX_OFFSET_X) // left & right
#define COLLISION_BOX_HEIGHT (SPRITES_HEIGHT - COLLISION_BOX_OFFSET_Y)

// ------------------------------------
// Maps
// ------------------------------------

#define TILES_WIDTH 32
#define TILES_HEIGHT 32

// The different types of maps found in a map file
// Background always fist, weather always last
typedef enum MapType MapType;
enum MapType {
    BACKGROUND,
    FOREGROUND,
    COLLISIONS,
    COST,
    WEATHER
};

// Maps structures
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
