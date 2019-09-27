#include "images.h"


static Image null_image()
{
    Image image;
    image.w = 0;
    image.h = 0;
    image.collision = FALSE;
    image.path = NULL;
    image.texture = NULL;
    return image;
}

static Image init_image(char const path[])
{
    Image image = null_image();

    TRY
    {
        image.path = calloc(sizeof(char), 30);
        if (image.path == NULL)
            THROW(TILES_PATH_MALLOC_FAILURE);
        strcpy(image.path, path);
    }
    CATCH(TILES_PATH_MALLOC_FAILURE)
    {
        logger(TILES_PATH_MALLOC_FAILURE, "");
        exit(EXIT_FAILURE);
    }
    ETRY;

    return image;
}

static Image load_texture_image(SDL_Renderer *renderer, char const filename[], char const path[])
{
    Image image = init_image(path);
    strcat(image.path, filename);
    SDL_Surface* surface = IMG_Load(image.path);
    if (surface != NULL)
    {
        surface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA8888, 0);
        image.texture = SDL_CreateTextureFromSurface(renderer, surface);
        image.w = surface->w;
        image.h = surface->h;
        SDL_FreeSurface(surface);
    }
    return image;
}

static void init_image_array(
        SDL_Renderer *renderer,
        unsigned int const min, unsigned int const max,
        Image array[max],
        char path[]
        )
{
    unsigned int i;
    char i_text[8] = {0};
    for (i=min;i<max;i++)
    {
        sprintf(i_text, "%04X.png", i);
        array[i] = load_texture_image(renderer, i_text, path);
    }
}

void init_tiles_array(SDL_Renderer *renderer, Image tiles[0xFFFF])
{
    unsigned int id;

    // start at 16, as first 15 will be used for infos & meta-images
    for (id=0;id<0x10;id++)
        tiles[id] = null_image();

    init_image_array(renderer, 0x10, 0xFFFF, tiles, TILES_IMAGES_PATH);

    // TODO: this should change when the array of collision image is defined
    for (id=0x10;id<0xFFFF;id++)
        tiles[id].collision = TRUE;
}

void init_mouse_array(SDL_Renderer *renderer, Image mouse[INVALID+1])
{
    mouse[EMPTY] = null_image();
    init_image_array(renderer, HOVER, INVALID+1, mouse, MOUSE_IMAGES_PATH);
}

void init_characters_array(SDL_Renderer *renderer, Image characters[4])
{
    init_image_array(renderer, 0, 4, characters, CHARACTERS_IMAGES_PATH);
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

static void free_image(Image *image)
{
    if (image->path != NULL)
    {
        free(image->path);
        if (image->texture != NULL)
            SDL_DestroyTexture(image->texture);
    }
}

void free_images_array(unsigned int const max, Image images[])
{
    unsigned int id;
    for (id=0;id<max;id++)
        free_image(&images[id]);
}

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
