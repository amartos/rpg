#include "errors.h"


// TODO: this function is only temporary, to change to make a log file
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

    switch (err_id)
    {
        case VIDEO_INIT_FAILURE:
            strcpy(err_msg, "Video init failure");
        case WINDOW_INIT_FAILURE:
            strcpy(err_msg, "Window init failure");
        case MAP_MALLOC_FAILURE:
            strcpy(err_msg, "Map memory allocation failure");
        case PATHFIND_MALLOC_FAILURE:
            strcpy(err_msg, "Pathfind memory allocation failure");
        case DATABASE_READ_FAILURE:
            strcpy(err_msg, "Database read failure");
        case QUERY_READ_FAILURE:
            strcpy(err_msg, "Query read failure");
        case QUERY_MODIFY_FAILURE:
            strcpy(err_msg, "Query modification failure");
        case QUERY_END_FAILURE:
            strcpy(err_msg, "Query end failure");
        case IMAGE_MALLOC_FAILURE:
            strcpy(err_msg, "Image memory allocation failure");
        case ANIMATION_MALLOC_FAILURE:
            strcpy(err_msg, "Animation memory allocation failure");
        case MOVEMENT_MALLOC_FAILURE:
            strcpy(err_msg, "Movement memory allocation failure");
        case ERR_MSG_MALLOC_FAILURE:
            strcpy(err_msg, "err_msg memory allocation failure");
        case TEXTURE_MALLOC_FAILURE:
            strcpy(err_msg, "Texture memory allocation failure");
        default:
            break;
    }

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
