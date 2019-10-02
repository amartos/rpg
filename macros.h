#ifndef RPG_MACROS
#define RPG_MACROS

#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>


// ------------------------------------
// Coding stuff
// ------------------------------------

#define get_name(var)  #var // get var name
#define MAX_SIZE_LINE 1000
#define DOUBLE_PRECISION 2

#define MALLOC(p, s, e, m) \
    TRY{p = malloc(s); if (p == NULL) {THROW(e);}} \
    CATCH_HANDLE(e, m) ETRY;

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
#define SCREEN_HEIGHT 720
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

#define TILES_WIDTH 64
#define TILES_HEIGHT 32


#define MAX_CHARACTERS 4

// Prototypes
Bool is_empty_string(char const string[]);
Bool is_same_double(double const a, double const b);

#endif // define RPG_MACROS
