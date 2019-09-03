#ifndef RPG_MAP
#define RPG_MAP

#include "errors.h"
#include "try_throw_catch.h"
#include "macros.h"

#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>


// prototypes
void init_map(Map *map, char const map_path[]);
void free_map(Map *map);

#endif // define RPG_MAP
