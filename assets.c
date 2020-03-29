#include "assets.h"


static void init_asset(Asset *asset)
{
    asset->image = NULL;
    asset->animation = NULL;
    asset->movement = NULL;
}

void init_asset_array(Asset assets[0xFFFF])
{
    for (int i=0;i<0xFFFF;i++)
        init_asset(&assets[i]);
}

void load_assets_db(SDL_Renderer *renderer, Asset assets[0xFFFF])
{
    int id, frames;
    double velocity;
    char path[50] = {0}, id_str[10] = {0};
    INIT_DB

    /* In the DB, the columns should be:
     * id < 0xFFFF | asset type | file path | number of frames | velocity */
    QUERY_DB("SELECT * FROM images;")
    {
        // IDs are basically numbers in hexadecimal between 0x0 and 0xFFFF.
        id = GET_QUERY_INT(0);
        sprintf(id_str, "id: %04X", id);

        strcpy(path,(const char*)GET_QUERY_TXT(2));
        MALLOC(assets[id].image, sizeof(Image), IMAGE_MALLOC_FAILURE, id_str);
        load_texture_image(renderer, assets[id].image, path);

        frames = GET_QUERY_INT(3);
        if (frames > 1)
        {
            MALLOC(assets[id].animation, sizeof(Animation), ANIMATION_MALLOC_FAILURE, id_str);
            init_animation(assets[id].animation, frames);
        }

        velocity = (double)GET_QUERY_INT(4);
        if (!is_same_double(velocity, 0.0))
        {
            // This cannot be done before, as velocity can be == 0 in db
            velocity = 1/velocity;
            MALLOC(assets[id].movement, sizeof(Movement), MOVEMENT_MALLOC_FAILURE, id_str);
            init_movement(assets[id].movement);
            assets[id].movement->velocity = velocity;
        }
    }
    END_QUERY
    CLOSE_DB
}

void free_assets_array(Asset assets[0xFFFF])
{
    for (int id=0;id<0xFFFF;id++)
        if (assets[id].image != NULL)
        {
            free_image(assets[id].image);
            free(assets[id].image);
            if (assets[id].animation != NULL)
                free(assets[id].animation);
            if (assets[id].movement != NULL)
                free(assets[id].movement);
        }
}
