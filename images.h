#ifndef RPG_IMAGES
#define RPG_IMAGES

#include "errors.h"
#include "try_throw_catch.h"
#include "macros.h"

#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define TILES_IMAGES_PATH "assets/tiles/"
#define MOUSE_IMAGES_PATH "assets/mouse/"
#define CHARACTERS_IMAGES_PATH "assets/sprites/characters/"

// structures
typedef enum Cursors Cursors;
enum Cursors
{
    EMPTY,
    HOVER,
    VALID,
    INVALID
};

typedef struct Image Image;
struct Image
{
    unsigned int w, h;
    Bool collision;
    char *path;
    SDL_Texture *texture;
};

// prototypes
void init_tiles_array(SDL_Renderer *renderer, Image tiles[0xFFFF]);
void init_mouse_array(SDL_Renderer *renderer, Image mouse[INVALID+1]);
void init_characters_array(SDL_Renderer *renderer, Image characters[4]);
SDL_Texture* make_colored_rect(
        SDL_Renderer *renderer,
        unsigned int const w, unsigned int const h,
        Uint8 const R, Uint8 const G, Uint8 const B, Uint8 const A
        );
void free_images_array(unsigned int const max, Image images[]);
void set_color(SDL_Surface *image, SDL_Color const original_color, SDL_Color const new_color);

#endif // define RPG_IMAGES
