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

#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

// prototypes
void init_screen(SDL_Surface *(*screen));
void set_BG_color(SDL_Surface *(*screen), const int rgb[3]);

#endif // define RPG_SCREEN
