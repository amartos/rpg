#ifndef RPG_ASSETS
#define RPG_ASSETS

#include "errors.h"
#include "macros.h"
#include "database.h"

#include "images.h"
#include "animation.h"
#include "movement.h"

#include <stdio.h>
#include <stdlib.h>

#define EMPTY_TILE_ID 0x0013

// Number of chars in team
#define MAX_CHARACTERS 4
#define FIRST_CHAR_ID 0x100
#define LAST_CHAR_ID FIRST_CHAR_ID+MAX_CHARACTERS-1

// Set cursors ID from DB
typedef enum Cursors Cursors;
enum Cursors
{
    EMPTY = 0,
    HOVER = 0x0010,
    VALID = 0x0011,
    INVALID = 0x012
};

/* This structure is a generic structure for an object, whatever it is.
 * All subtypes are init to NULL with the init_asset functions, and modified if
 * needed depending on the DB given. */
typedef struct Asset Asset;
struct Asset
{
    Image *image;
    Animation *animation;
    Movement *movement;
};


/* As multiple assets are used, we only need an array of them. This function
 * initialize the array. */
void init_asset_array(Asset assets[0xFFFF]);

/* This function will fill the assets array with info from the DB which path is
 * defined in the database module. */
void load_assets_db(SDL_Renderer *renderer, Asset assets[0xFFFF]);

// This frees all the malloc in assets.
void free_assets_array(Asset assets[0xFFFF]);

#endif // define RPG_ASSETS
