#ifndef RPG_ASSETS
#define RPG_ASSETS

#include <stdio.h>
#include <stdlib.h>

#include "images.h"
#include "animation.h"
#include "movement.h"


// structures
typedef struct Asset Asset;
struct Asset
{
    Image *image;
    Animation *animation;
    Movement *movement;
};


// prototypes
void init_asset_array(Asset assets[0xFFFF]);
void free_assets_array(Asset assets[0xFFFF]);

#endif // define RPG_ASSETS
