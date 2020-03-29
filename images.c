#include "images.h"


void init_sdl_rect(SDL_Rect *rect)
{
    rect->x = 0; rect->y = 0;
    rect->w = 0; rect->h = 0;
}

void set_image_size(SDL_Rect *rect, int const w, int const h)
{
    rect->w = w; rect->h = h;
}

void set_image_position(SDL_Rect *rect, int const x, int const y)
{
    rect->x = x; rect->y = y;
}

SDL_Rect init_frame_sdlrect(int const x, int const y)
{
    SDL_Rect rect; init_sdl_rect(&rect);
    set_image_position(&rect, x*FRAMES_WIDTH, y*FRAMES_HEIGHT);
    set_image_size(&rect, FRAMES_WIDTH, FRAMES_HEIGHT);
    return rect;
}

/* This is private because one should only init the Image structure by using the
 * load_texture_image and giving path. */
static void init_image(Image *image)
{
    init_sdl_rect(&image->sdlrect);
    image->collision = FALSE;
    for (int i=0;i<MAX_PATH_LEN;i++)
        image->path[i] = '\0';
    MALLOC(image->texture, sizeof(SDL_Texture*), TEXTURE_MALLOC_FAILURE, NULL);
}

void load_texture_image(SDL_Renderer *renderer, Image *image, char const path[MAX_PATH_LEN])
{
    init_image(image); strcpy(image->path, path);
    SDL_Surface* surface = IMG_Load(image->path);
    if (surface != NULL)
    {
        surface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA8888, 0);
        image->texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
    }
}

void free_image(Image *image)
{
    if (image->texture != NULL)
        SDL_DestroyTexture(image->texture);
}
