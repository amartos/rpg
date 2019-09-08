#include "images.h"


void set_color(SDL_Surface *image, SDL_Color const original_color, SDL_Color const new_color)
{
    unsigned int w = image->w, h = image->h;
    unsigned int total_pixels = w * h, i;
    Uint8 r, g, b, a;
    Uint32 *pixels = (Uint32 *)image->pixels;

    for (i=0;i<total_pixels;i++)
    {
        Uint32 pixel = *((Uint32 *)image->pixels + i);
        SDL_GetRGBA(pixel, image->format, &r, &g, &b, &a);
        if(r == original_color.r && g == original_color.g && b == original_color.b)
            pixels[i] = SDL_MapRGBA(image->format, new_color.r, new_color.g, new_color.b, new_color.a);
    }
}
