#include "characters.h"


static void init_on_screen(SDL_Renderer **renderer, OnScreen *on_screen, unsigned int const id)
{
    unsigned int i, j, f;
    SDL_Surface *sprite_surface = NULL;

    //-------------------------------------------------------------------------
    // Sprites loading & colorization
    //-------------------------------------------------------------------------

    TRY
    {
        sprite_surface = IMG_Load("assets/sprites/characters/test_character_grey.png");
        if (sprite_surface == NULL)
            THROW(SPRITE_LOAD_FAILURE);
        // double size of sprites as the images are really small
        // 16 pixels w/h is too small for recent screens but
        // good for GBC, and the test sprites are from this console
        // this will be delete when real sprites are done
        sprite_surface = SDL_ConvertSurfaceFormat(sprite_surface, SDL_PIXELFORMAT_RGBA8888, 0);
        sprite_surface = rotozoomSurface(sprite_surface, 0.0, 2.0, 0.0);
    }
    CATCH(SPRITE_LOAD_FAILURE)
    {
        logger(SPRITE_LOAD_FAILURE, SDL_GetError());
        exit(EXIT_FAILURE);
    }
    ETRY;

    // in order to be able to change colors of sprites depending on the day or
    // others, the sprites need to be in shades of grey. Thus this sets the colors

    // for now this is half-manual, but LUT will be used in the future
    SDL_Color greys[COLOR_PALETTE];
    greys[0].r = 0x91, greys[0].g = 0x91, greys[0].b = 0x91;
    greys[1].r = 0xC6, greys[1].g = 0xC6, greys[1].b = 0xC6;
    greys[2].r = 0x7f, greys[2].g = 0x7f, greys[2].b = 0x7f;

    // load characters palette : 0 green, 1 red, 2 blue, 3 yellow
    SDL_Color palette[COLOR_PALETTE];
    palette[1].r = 0xF8, palette[1].g = 0xB8, palette[1].b = 0x88;
    palette[2].r = 0x18, palette[2].g = 0x80, palette[2].b = 0xF8;
    switch (id)
    {
        case GREEN:
            palette[0].r = 0x10, palette[0].g = 0xA8, palette[0].b = 0x40;
            break;
        case RED:
            palette[0].r = 0xF8, palette[0].g = 0x00, palette[0].b = 0x00;
            break;
        case BLUE:
            palette[0].r = 0x18, palette[0].g = 0x80, palette[0].b = 0xf8;
            break;
        case YELLOW:
            palette[0].r = 0xf0, palette[0].g = 0xe8, palette[0].b = 0x18;
            break;
    }

    for (i=0;i<COLOR_PALETTE;i++)
    {
        palette[i].a = 0xFF;
        set_color(sprite_surface, greys[i], palette[i]);
    }

    on_screen->sprite = SDL_CreateTextureFromSurface(*renderer, sprite_surface);
    SDL_FreeSurface(sprite_surface);

    //-------------------------------------------------------------------------
    // Frames
    //-------------------------------------------------------------------------

    on_screen->total_frames = 2;

    // This depends on the sprite order
    // TODO: if(sprite changes), reorder
    for (i=N;i<=NW;i++) // directions
        for (j=MOVE;j<=MOVE_SHIELD;j++) // States
            for (f=0;f<on_screen->total_frames;f++)
            {
                on_screen->frames[i][j][f].x = (i * 2 + f) * SPRITES_WIDTH;
                on_screen->frames[i][j][f].y = j * SPRITES_HEIGHT;
                on_screen->frames[i][j][f].w = SPRITES_WIDTH;
                on_screen->frames[i][j][f].h = SPRITES_HEIGHT;
            }

    on_screen->current_frame = 0;

    //-------------------------------------------------------------------------
    // Constants
    //-------------------------------------------------------------------------

    on_screen->animated = FALSE;
    on_screen->framerate = 0;
    if (on_screen->total_frames > 1)
    {
        on_screen->animated = TRUE;
        on_screen->framerate = 1000/12; // 1000/character FPS
    }

    on_screen->state = MOVE;
    on_screen->time = 0;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void init_character(
        SDL_Renderer **renderer,
        Character *character,
        unsigned int const id,
        Coord const start_position,
        Deployment const formation
        )
{
    character->id = id;
    init_movement(&character->movement, start_position, formation, id);
    init_on_screen(renderer, &character->on_screen, id);
}
