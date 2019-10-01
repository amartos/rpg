#ifndef RPG_IMAGES
#define RPG_IMAGES

#include "errors.h"
#include "try_throw_catch.h"
#include "macros.h"

#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define IMAGES_PATH "assets/images/"


// structures
typedef enum Cursors Cursors;
enum Cursors
{
    EMPTY = 0,
    HOVER = 0x0010,
    VALID = 0x0011,
    INVALID = 0x012
};

typedef struct Image Image;
struct Image
{
    unsigned int w, h;
    Bool collision;
    char path[50];
    SDL_Texture *texture;
};

// prototypes
void load_texture_image(SDL_Renderer *renderer, Image *image, char const path[]);
SDL_Texture* make_colored_rect(
        SDL_Renderer *renderer,
        unsigned int const w, unsigned int const h,
        Uint8 const R, Uint8 const G, Uint8 const B, Uint8 const A
        );
void free_image(Image *image);

#endif // define RPG_IMAGES
