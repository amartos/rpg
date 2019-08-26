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

    character->collision_box.x = character->infos.x + COLLISION_BOX_OFFSET_X;
    character->collision_box.y = character->infos.y + COLLISION_BOX_OFFSET_Y;
    character->collision_box.w = COLLISION_BOX_WIDTH;
    character->collision_box.h = COLLISION_BOX_HEIGHT;

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

static int check_collisions(const Character character, const int direction, const Map map)
{
    int wall = FALSE, i, j, k, m; // k is i' and m is j'
    SDL_Rect curr_pos = character.infos, next_pos;
    int velocity = character.velocity;
    switch(direction)
    {
        case UP:
            next_pos.x = curr_pos.x + COLLISION_BOX_OFFSET_X;
            next_pos.y = curr_pos.y + COLLISION_BOX_OFFSET_Y - velocity;
            break;
        case DOWN:
            next_pos.x = curr_pos.x + COLLISION_BOX_OFFSET_X;
            next_pos.y = curr_pos.y + COLLISION_BOX_OFFSET_Y + velocity;
            break;
        case LEFT:
            next_pos.x = curr_pos.x + COLLISION_BOX_OFFSET_X - velocity;
            next_pos.y = curr_pos.y + COLLISION_BOX_OFFSET_Y;
            break;
        case RIGHT:
            next_pos.x = curr_pos.x + COLLISION_BOX_OFFSET_X + velocity;
            next_pos.y = curr_pos.y + COLLISION_BOX_OFFSET_Y;
            break;
    }

    i = next_pos.x/TILES_WIDTH;
    j = next_pos.y/TILES_HEIGHT;
    if (map.collisions[i][j])
        wall = TRUE;

    k = (next_pos.x + character.collision_box.w) / TILES_WIDTH;
    m = (next_pos.y + character.collision_box.h) / TILES_HEIGHT;
    if (map.collisions[k][m])
        wall = TRUE;

    return wall;
}

void place_character(Character *character, const int x, const int y)
{
    character->infos.x = x;
    character->infos.y = y;
    character->collision_box.x = x+COLLISION_BOX_OFFSET_X;
    character->collision_box.x = x+COLLISION_BOX_OFFSET_Y;
}

void move_character(Character *character, const int direction, const int current_time, const Map map)
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
                        if (!check_collisions(*character, direction, map))
                            character->infos.y -= character->velocity;
                        break;
                    case DOWN:
                        if (!check_collisions(*character, direction, map))
                            character->infos.y += character->velocity;
                        break;
                    case LEFT:
                        if (!check_collisions(*character, direction, map))
                            character->infos.x -= character->velocity;
                        break;
                    case RIGHT:
                        if (!check_collisions(*character, direction, map))
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
