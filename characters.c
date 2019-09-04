#include "characters.h"


static void init_on_screen(OnScreen *on_screen, unsigned int const id)
{
    unsigned int i, j, f;
    SDL_Surface *sprite = NULL;

    //-------------------------------------------------------------------------
    // Sprites loading & colorization
    //-------------------------------------------------------------------------

    TRY
    {
        sprite = IMG_Load("assets/sprites/characters/test_character_grey.png");
        if (sprite == NULL)
            THROW(SPRITE_LOAD_FAILURE);
        // double size of sprites as the images are really small
        // 16 pixels w/h is too small for recent screens but
        // good for GBC, and the test sprites are from this console
        // this will be delete when real sprites are done
        on_screen->sprite = rotozoomSurface(sprite, 0.0, 2.0, 0.0);
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
        set_color(on_screen->sprite, greys[i], palette[i]);

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

static void init_movement(
        Movement *movement,
        Coord const start_position,
        Deployment const formation,
        unsigned int const id
        )
{
    unsigned int i;
    init_coord(&movement->position);
    movement->position = start_position;
    formation_offsetting(&movement->position, id, formation);
    movement->formation = formation;
    for (i=0;i<MAX_PATH_NODES;i++)
        init_coord(&movement->path[i]);
    movement->moving = FALSE;
    movement->velocity = 4;
    movement->direction = S;
    movement->current_node = 0;
    movement->movement_type = WALK;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void init_character(Character *character, unsigned int const id, Coord const start_position, Deployment const formation)
{
    character->id = id;
    init_movement(&character->movement, start_position, formation, id);
    init_on_screen(&character->on_screen, id);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void free_character(Character *character)
{
    SDL_FreeSurface(character->on_screen.sprite);
}
