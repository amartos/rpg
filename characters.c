#include "characters.h"

void init_character(
        Character *character,
        const char sprite_path[],
        const int number_of_frames,
        const int moving,
        const int fps,
        const int velocity
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
    character->moving = moving;
    character->current_frame = 0;
    character->previous_time = 0;
}

void free_character(Character *character)
{
    SDL_FreeSurface(character->sprite);
}

void move_character(Character *character, const int direction, const int current_time)
{
    TRY
    {
        if (character->animated)
        {
            if (character->moving)
            {
                switch(direction)
                {
                    case UP:
                        character->infos.y -= character->velocity;
                        break;
                    case DOWN:
                        character->infos.y += character->velocity;
                        break;
                    case LEFT:
                        character->infos.x -= character->velocity;
                        break;
                    case RIGHT:
                        character->infos.x += character->velocity;
                        break;
                }
                character->direction = direction;
                if (current_time - character->previous_time > character->framerate)
                {
                    (character->current_frame)++;
                    if (character->current_frame >= character->number_of_frames)
                        character->current_frame = 0;
                    character->previous_time = current_time;
                }
            }
            else
                THROW(CHARACTER_NOT_MOVING);
        }
        else
            THROW(CHARACTER_NOT_ANIMATED);
    }
    CATCH(CHARACTER_NOT_ANIMATED)
    {
        char message[100] = {0};
        sprintf(message, "%s is not animated", get_name(character));
        logger(CHARACTER_NOT_ANIMATED, message);
    }
    CATCH(CHARACTER_NOT_MOVING)
    {
        char message[100] = {0};
        sprintf(message, "%s cannot move", get_name(character));
        logger(CHARACTER_NOT_ANIMATED, message);
    }
    ETRY;
}
