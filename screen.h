#ifndef RPG_SCREEN
#define RPG_SCREEN

#include "errors.h"
#include "macros.h"

#include "coord.h"
#include "map.h"
#include "assets.h"
#include "images.h" // for FRAMES dim
#include "animation.h" // only for FPS

#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

/* Screen dimensions
 * TODO: define this with a function checking the real size */
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define SCREEN_BPP 32
#define FRAMERATE 1000/FPS
// Autoscale tiles sizes with camera settings
#define TILES_WIDTH(c) scale_size(BASE_TILES_WIDTH, c)
#define TILES_HEIGHT(c) scale_size(BASE_TILES_HEIGHT, c)
// default camera center position
#define DEFAULT_CAM_X(c) (SCREEN_WIDTH/2)/TILES_WIDTH(c)
#define DEFAULT_CAM_Y(c) (SCREEN_HEIGHT/2)/TILES_HEIGHT(c)

// min & max scale for camera
#define MAX_ZOOM 4
#define MIN_ZOOM 0.5

/* This function inits the window and renderer and throw errors in case it
 * catches one. */
void init_screen(SDL_Window **window, SDL_Renderer **renderer);

// This structure is used to set the zoom and reduce the shown map on screen.
typedef struct Camera Camera;
struct Camera
{
    Coord scroll; // camera position on map
    double scale;
};

// This inits the Camera struct to 0.000 for all.
void init_camera(Camera *camera);

// Converts size to the right camera scale and returns the new size as double.
double scale_size(unsigned int const size, Camera const camera);

// Modify scroll.x by xmod and scroll.y by ymod
void set_scroll(Camera *camera, int const xmod, int const ymod);

/* Modify camera->scale by 2 if out is FALSE or 2^-1 if out is TRUE
 * Use MAX_ZOOM and MIN_ZOOM as maximum values */
void set_zoom(Camera *camera, Bool const out);

// reset camera values to init
void reset_camera(Camera *camera);

// set camera scroll to DEFAULT_CAM_POS
void center_camera(Camera *camera);

/* Convert (x,y) screen isometric coord to cartesian (x,y) Coord
 * taking account of the camera position and scale */
Coord screen_to_coord(int const x, int const y, Camera const camera);

/* This function will draw everything on screen where it should be, and
 * depending on where the camera is placed. */
void render_screen(
        SDL_Renderer *renderer,
        Asset assets[0xFFFF],
        Cursors mouse_type,
        Camera camera,
        Map const map,
        Bool paused
        );

#endif // define RPG_SCREEN
