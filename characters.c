#include "characters.h"


void init_character(
        Character *character,
        SDL_Color const colors[],
        char const sprite_path[],
        unsigned int const number_of_frames,
        unsigned int const fps,
        unsigned int const velocity,
        Coord const start_position
        )
{
    unsigned int i, j, f;

    character->state = MOVE;
    character->direction = S;
    character->nodes = 0;
    character->current_frame = 0;
    character->previous_time = 0;

    character->movement_type = WALK;
    character->moving = FALSE;
    character->number_of_frames = number_of_frames;
    if (number_of_frames > 1)
    {
        character->animated = TRUE;
        character->framerate = 1000/fps;
        character->velocity = velocity;
    }
    else
    {
        character->animated = FALSE;
        character->framerate = 0;
        character->velocity = 0;
    }

    // Coord position
    character->position.x = start_position.x;
    character->position.y = start_position.y;
    character->position.ox = start_position.ox;
    character->position.oy = start_position.oy;

    // Coord goal
    character->goal.x = 0;
    character->goal.y = 0;
    character->goal.ox = 0;
    character->goal.oy = 0;

    // Coord *path
    character->path = NULL;

    // SDL_Surface *sprite
    TRY
    {
        character->sprite = IMG_Load(sprite_path);
        if (character->sprite == NULL)
            THROW(SPRITE_LOAD_FAILURE);
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

    for (i=0;i<COLOR_PALETTE;i++)
        set_color(character->sprite, greys[i], colors[i]);

    // SDL_Rect ***frames
    // This depends on the sprite order
    // TODO: if(sprite changes), reorder
    for (i=W;i<=E;i++)
        for (j=MOVE;j<=MOVE_SHIELD;j++)
            for (f=0;f<number_of_frames;f++)
            {
                character->frames[i][j][f].x = (i * 2 + f) * SPRITES_WIDTH;
                character->frames[i][j][f].y = j * SPRITES_HEIGHT;
                character->frames[i][j][f].w = SPRITES_WIDTH;
                character->frames[i][j][f].h = SPRITES_HEIGHT;
            }
}

void free_character(Character *character)
{
    SDL_FreeSurface(character->sprite);
    if (character->path != NULL)
        free_path(character);
}

void free_path(Character *character)
{
    free(character->path);
    character->path = NULL;
}
