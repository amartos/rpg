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

void init_images_array(SDL_Renderer *renderer, Image images[0xFFFF])
{
    unsigned int id;
    char id_text[8] = {0};

    // start at 16, as first 15 will be used for infos & meta-images
    for (id=0x10;id<0xFFFF;id++)
    {
        sprintf(id_text, "%04X.png", id);
        images[id] = load_texture_image(renderer, id_text, IMAGES_PATH);
    }

    // TODO: this should change when the array of collision image is defined
    for (id=0x400;id<0x4000;id++)
        images[id].collision = TRUE;
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

void free_images_array(Image images[])
{
    unsigned int id;
    for (id=0;id<0xFFFF;id++)
        free_image(&images[id]);
}
