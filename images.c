#include "images.h"


static Tile null_tile()
{
    Tile tile;
    tile.w = 0;
    tile.h = 0;
    tile.collision = FALSE;
    tile.path = NULL;
    tile.texture = NULL;
    return tile;
}

static Tile init_tile()
{
    Tile tile = null_tile();

    TRY
    {
        tile.path = calloc(sizeof(char), 30);
        if (tile.path == NULL)
            THROW(TILES_PATH_MALLOC_FAILURE);
        strcpy(tile.path, TILES_PATH);
    }
    CATCH(TILES_PATH_MALLOC_FAILURE)
    {
        logger(TILES_PATH_MALLOC_FAILURE, "");
        exit(EXIT_FAILURE);
    }
    ETRY;

    return tile;
}

static Tile load_texture_tile(SDL_Renderer *renderer, char const filename[])
{
    Tile tile = init_tile();
    strcat(tile.path, filename);
    SDL_Surface* surface = IMG_Load(tile.path);
    if (surface != NULL)
    {
        surface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA8888, 0);
        tile.texture = SDL_CreateTextureFromSurface(renderer, surface);
        tile.w = surface->w;
        tile.h = surface->h;
        SDL_FreeSurface(surface);
    }
    return tile;
}

void init_tiles_array(SDL_Renderer *renderer, Tile tiles[0xFFFF])
{
    unsigned int id;
    char id_text[8] = {0};

    // start at 16, as first 15 will be used for infos & meta-tiles
    for (id=0;id<0x10;id++)
        tiles[id] = null_tile();

    for (id=0x10;id<0xFFFF;id++)
    {
        sprintf(id_text, "%04X.png", id);
        tiles[id] = load_texture_tile(renderer, id_text);
    }

    // TODO: this should change when the array of collision tile is defined
    for (id=0x10;id<0xFFFF;id++)
        tiles[id].collision = TRUE;
}

static void free_tile(Tile *tile)
{
    if (!is_empty_string(tile->path))
    {
        free(tile->path);
        if (tile->texture != NULL)
            SDL_DestroyTexture(tile->texture);
    }
}

void free_tiles_array(Tile tiles[0xFFFF])
{
    unsigned int id;
    for (id=0x10;id<0xFFFF;id++)
        free_tile(&tiles[id]);
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
