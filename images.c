#include "images.h"


static void init_image(Image *image)
{
    unsigned int i;
    image->w = 0;
    image->h = 0;
    image->collision = FALSE;
    for (i=0;i<50;i++)
        image->path[i] = '\0';
    image->texture = malloc(sizeof(SDL_Texture*));
}

void load_texture_image(SDL_Renderer *renderer, Image *image, char const path[])
{
    init_image(image); strcpy(image->path, path);
    SDL_Surface* surface = IMG_Load(image->path);
    if (surface != NULL)
    {
        surface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA8888, 0);
        image->texture = SDL_CreateTextureFromSurface(renderer, surface);
        image->w = surface->w;
        image->h = surface->h;
        SDL_FreeSurface(surface);
    }
}

SDL_Texture* make_colored_rect(
        SDL_Renderer *renderer,
        unsigned int const w, unsigned int const h,
        Uint8 const R, Uint8 const G, Uint8 const B, Uint8 const A
        )
{
    SDL_Surface *surface = SDL_CreateRGBSurface(0, w, h, SCREEN_BPP, 0, 0, 0, 0);
    SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, R, G, B));
    surface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA8888, 0);

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
	SDL_SetTextureAlphaMod(texture, A);

    return texture;
}

void free_image(Image *image)
{
    if (image->texture != NULL)
        SDL_DestroyTexture(image->texture);
}
