#ifndef RPG_IMAGES
#define RPG_IMAGES

#include "errors.h"
#include "macros.h"

#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

// isometric tiles dimensions
#define BASE_TILES_WIDTH 64
#define BASE_TILES_HEIGHT 32
/* The tiles and characters images are from two different sets, and chars are
 * not isometric. Theses dimensions are used for characters sprites
 * TODO: unify dimensions for images. This implies to use one sprite per type of
 * image (ground, etc), and use the init_frame_sdlrect to locate the right
 * image */
#define FRAMES_WIDTH 128
#define FRAMES_HEIGHT 128

/* default images path within the source dir.
 * This will change for the installation path once it is implemented. */
#define IMAGES_PATH "assets/images/"
#define MAX_PATH_LEN 50


// An image structure used to gather datas about each images.
typedef struct Image Image;
struct Image
{
    SDL_Rect sdlrect;
    Bool collision;
    char path[50];
    SDL_Texture *texture;
};

// This function sets all values of SDL_Rect to 0.
void init_sdl_rect(SDL_Rect *rect);

// Modify the SDL_Rect structure
void set_image_position(SDL_Rect *rect, int const x, int const y);
void set_image_size(SDL_Rect *rect, int const w, int const h);

/* return the SDL_Rect structure for the frame on position x and y
 * x and y are NOT in pixel, they correspond to the frame position on the
 * sprite. These values are multiplied by the FRAMES dims to get the pixel
 * values within the function itself. */
SDL_Rect init_frame_sdlrect(int const x, int const y);

// Fills the Image structure with an image from path, stored as an SDL_Texture
void load_texture_image(SDL_Renderer *renderer, Image *image, char const path[MAX_PATH_LEN]);

/* A basic alias to SDL_DestroyTexture that checks if the Image->SDL_Texture is
 * NULL before calling the function. */
void free_image(Image *image);

#endif // define RPG_IMAGES
