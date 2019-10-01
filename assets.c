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
