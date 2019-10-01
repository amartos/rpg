#include "assets.h"


static void init_asset(Asset *asset)
{
    asset->image = NULL;
    asset->animation = NULL;
    asset->movement = NULL;
}

void init_asset_array(Asset assets[0xFFFF])
{
    unsigned int i;
    for (i=0;i<0xFFFF;i++)
        init_asset(&assets[i]);
}

void load_assets_db(SDL_Renderer *renderer, Asset assets[0xFFFF])
{
    unsigned int id, frames;
    double velocity;
    char path[50] = {0};
    INIT_DB

    QUERY_DB("SELECT * FROM images;")
    {
        id = GET_QUERY_INT(0);

        strcpy(path,(const char*)GET_QUERY_TXT(2));
        // TODO: better handle errors
        TRY
        {
            assets[id].image = malloc(sizeof(Image));
            if (assets[id].image == NULL)
                THROW(IMAGE_MALLOC_FAILURE);
        }
        CATCH(IMAGE_MALLOC_FAILURE)
        {
            printf("oups\n");
        }
        ETRY;
        load_texture_image(renderer, assets[id].image, path);

        frames = GET_QUERY_INT(3);
        if (frames > 1)
        {
            TRY
            {
                assets[id].animation = malloc(sizeof(Animation));
                if (assets[id].animation == NULL)
                    THROW(IMAGE_MALLOC_FAILURE);
                init_animation(assets[id].animation, frames);
            }
            CATCH(IMAGE_MALLOC_FAILURE)
            {
                printf("oups\n");
            }
            ETRY;
        }

        velocity = GET_QUERY_INT(4);
        if (!is_same_double(velocity, 0.0))
        {
            // This cannot be done before, as velocity can be == 0 in db
            velocity = 1/velocity;
            TRY
            {
                assets[id].movement = malloc(sizeof(Movement));
                if (assets[id].movement == NULL)
                    THROW(IMAGE_MALLOC_FAILURE);
            }
            CATCH(IMAGE_MALLOC_FAILURE)
            {
                printf("oups\n");
            }
            ETRY;
            init_movement(assets[id].movement);
            assets[id].movement->velocity = velocity;
        }
    }
    END_QUERY
    CLOSE_DB
}

void free_assets_array(Asset assets[0xFFFF])
{
    unsigned int id;
    for (id=0;id<0xFFFF;id++)
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
