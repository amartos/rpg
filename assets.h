#ifndef RPG_ASSETS
#define RPG_ASSETS

#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>

#include "errors.h"
#include "try_throw_catch.h"
#include "macros.h"

#include "database.h"

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
void load_assets_db(SDL_Renderer *renderer, Asset assets[0xFFFF]);
void free_assets_array(Asset assets[0xFFFF]);

#endif // define RPG_ASSETS
