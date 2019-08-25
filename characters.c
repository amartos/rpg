#include "characters.h"

void init_character(Character *character, const char sprite_path[], const int number_of_frames, const int fps, const int velocity)
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

    character->state = MOVE;
    character->direction = DOWN;
    character->infos.x = 0;
    character->infos.y = 0;
    character->infos.w = SPRITES_WIDTH;
    character->infos.h = SPRITES_HEIGHT;
    character->number_of_frames = number_of_frames;

    int f, i, j;
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
    character->current_frame = 0;
}

void free_character(Character *character)
{
    SDL_FreeSurface(character->sprite);
}
