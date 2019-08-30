#include "characters.h"

void init_character(
        Character *character,
        SDL_Color colors[],
        const char sprite_path[],
        const int number_of_frames,
        const int moving,
        const int fps,
        const int velocity,
        const Coord start_position
        )
{
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

    /* in order to be able to change colors of sprites depending on the day or
       others, the sprites need to be in shades of grey. Thus this sets the colors */

    int i, j, f;

    // for now this is half-manual, but LUT will be used in the future
    SDL_Color greys[COLOR_PALETTE];
    greys[0].r = 0x91, greys[0].g = 0x91, greys[0].b = 0x91;
    greys[1].r = 0xC6, greys[1].g = 0xC6, greys[1].b = 0xC6;
    greys[2].r = 0x7f, greys[2].g = 0x7f, greys[2].b = 0x7f;

    for (i=0;i<COLOR_PALETTE;i++)
        set_color(character->sprite, greys[i], colors[i]);

    character->state = MOVE;
    character->direction = DOWN;
    character->infos.x = start_position.x;
    character->infos.y = start_position.y;
    character->infos.w = SPRITES_WIDTH;
    character->infos.h = SPRITES_HEIGHT;

    character->position.x = start_position.x;
    character->position.y = start_position.x;
    character->goal.x = 0;
    character->goal.y = 0;

    character->number_of_frames = number_of_frames;

    // This depends on the sprite order
    // TODO: if(sprite changes), reorder
    for (i=LEFT;i<=RIGHT;i++)
    {
        for (j=MOVE;j<=MOVE_SHIELD;j++)
        {
            for (f=0;f<number_of_frames;f++)
            {
                character->frames[i][j][f].x = (i * 2 + f) * SPRITES_WIDTH;
                character->frames[i][j][f].y = j * SPRITES_HEIGHT;
                character->frames[i][j][f].w = SPRITES_WIDTH;
                character->frames[i][j][f].h = SPRITES_HEIGHT;
            }
        }
    }

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
    character->moving = moving;
    character->current_frame = 0;
    character->previous_time = 0;
}

void free_character(Character *character)
{
    SDL_FreeSurface(character->sprite);
}
