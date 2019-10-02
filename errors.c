#include "errors.h"


// this is only temporary, to change in log in file
static void printe(
        time_t const now,
        Errors const err_id,
        char const *err_msg,
        char const *message
        )
{
    printf("[%ld] %s (%d)", now, err_msg, err_id);
    if (message != NULL)
        printf(": %s", message);
    printf("\n");
}

static char* errstr(Errors const err_id)
{
    char *err_msg = NULL;
    MALLOC(err_msg, sizeof(char) * 50, ERR_MSG_MALLOC_FAILURE, NULL);

    if (err_id == VIDEO_INIT_FAILURE)
        strcpy(err_msg, "Video init failure");
    else if (err_id == WINDOW_INIT_FAILURE)
        strcpy(err_msg, "Window init failure");
    else if (err_id == MAP_MALLOC_FAILURE)
        strcpy(err_msg, "Map memory allocation failure");
    else if (err_id == PATHFIND_MALLOC_FAILURE)
        strcpy(err_msg, "Pathfind memory allocation failure");
    else if (err_id == DATABASE_READ_FAILURE)
        strcpy(err_msg, "Database read failure");
    else if (err_id == QUERY_READ_FAILURE)
        strcpy(err_msg, "Query read failure");
    else if (err_id == QUERY_END_FAILURE)
        strcpy(err_msg, "Query end failure");
    else if (err_id == IMAGE_MALLOC_FAILURE)
        strcpy(err_msg, "Image memory allocation failure");
    else if (err_id == ANIMATION_MALLOC_FAILURE)
        strcpy(err_msg, "Animation memory allocation failure");
    else if (err_id == MOVEMENT_MALLOC_FAILURE)
        strcpy(err_msg, "Movement memory allocation failure");
    else if (err_id == ERR_MSG_MALLOC_FAILURE)
        strcpy(err_msg, "err_msg memory allocation failure");
    else if (err_id == TEXTURE_MALLOC_FAILURE)
        strcpy(err_msg, "Texture memory allocation failure");

    return err_msg;
}

static void err_action(Errors const err_id)
{
    switch (err_id)
    {
        case VIDEO_INIT_FAILURE:
        case WINDOW_INIT_FAILURE:
        case MAP_MALLOC_FAILURE:
        case PATHFIND_MALLOC_FAILURE:
        case DATABASE_READ_FAILURE:
        case QUERY_READ_FAILURE:
        case QUERY_END_FAILURE:
        case ERR_MSG_MALLOC_FAILURE:
        case TEXTURE_MALLOC_FAILURE:
            exit(EXIT_FAILURE);
            break;
        default:
            break;
    }
}

static void logger(Errors const err_id, char const *message)
{
    time_t now; time(&now);
    char *err_msg = errstr(err_id);
    if (err_msg == NULL)
        printe(now, err_id, "NULL ERR_MSG", NULL);
    else
        printe(now, err_id, err_msg, message);
    free(err_msg);
}

void handle_error(Errors const err_id, char const *message)
{
    logger(err_id, message);
    err_action(err_id);
}
