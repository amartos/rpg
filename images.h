#ifndef RPG_IMAGES
#define RPG_IMAGES

#include "errors.h"
#include "try_throw_catch.h"
#include "macros.h"

#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define TILES_PATH "assets/tiles/"

// structures
typedef enum Cursors Cursors;
enum Cursors
{
    EMPTY,
    HOVER,
    VALID,
    INVALID
};

typedef struct Tile Tile;
struct Tile
{
    unsigned int w, h;
    Bool collision;
    char *path;
    SDL_Texture *texture;
};

// prototypes
void init_tiles_array(SDL_Renderer *renderer, Tile tiles[0xFFFF]);
void free_tiles_array(Tile tiles[0xFFFF]);
void set_color(SDL_Surface *image, SDL_Color const original_color, SDL_Color const new_color);

#endif // define RPG_IMAGES
