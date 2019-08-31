#include "images.h"


void set_color(SDL_Surface *image, SDL_Color original_color, SDL_Color new_color)
{
    int w = image->w, h = image->h;
    int total_pixels = w * h, i;
    Uint8 r, g, b;
    Uint32 *pixels = (Uint32 *)image->pixels;

    for (i=0;i<total_pixels;i++)
    {
        Uint32 pixel = *((Uint32 *)image->pixels + i);
        SDL_GetRGB(pixel, image->format, &r, &g, &b);
        if(r == original_color.r && g == original_color.g && b == original_color.b)
            pixels[i] = SDL_MapRGB(image->format, new_color.r, new_color.g, new_color.b);
    }
}
