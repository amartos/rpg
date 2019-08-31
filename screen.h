#ifndef RPG_SCREEN
#define RPG_SCREEN

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 640
#define SCREEN_BPP 32
#define BG_COLOR 0xFF, 0xFF, 0xFF
#define FPS 30
#define FRAMERATE 1000/FPS

#include "errors.h"
#include "try_throw_catch.h"
#include "macros.h"
#include "map.h"

#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

// structures
enum {R, G, B};

// prototypes
void init_screen(SDL_Surface *(*screen));
void set_BG_color(SDL_Surface *(*screen), const int rgb[3]);

// SDL_Surface tile will change for an array once more tiles are included
void apply_tiles(SDL_Surface *(*screen), MapType type, Map map, SDL_Surface *tile);

#endif // define RPG_SCREEN
