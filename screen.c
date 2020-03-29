#include "screen.h"


void init_screen(SDL_Window **window, SDL_Renderer **renderer)
{
    TRY
    {
        if (SDL_Init(SDL_INIT_VIDEO))
            THROW(VIDEO_INIT_FAILURE);
        if (
                SDL_CreateWindowAndRenderer(
                800, 600, // to replace with FULL_SCREEN_DESKTOP
                SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED,
                window, renderer
                )
           )
            THROW(WINDOW_INIT_FAILURE);
    }
    CATCH_HANDLE(SDL_INIT_VIDEO, SDL_GetError())
    CATCH_HANDLE(WINDOW_INIT_FAILURE, SDL_GetError())
    ETRY;

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear"); // softer resizes 
    SDL_RenderSetLogicalSize(*renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
    SDL_SetRenderDrawBlendMode(*renderer, SDL_BLENDMODE_BLEND);
}

void init_camera(Camera *camera)
{
    init_coord(&camera->scroll);
    camera->scale = 1;
}

int scale_size(int const size, Camera const camera)
{
    return (int)(size * camera.scale);
}

void set_scroll(Camera *camera, int const xmod, int const ymod)
{
    camera->scroll.x += (double)xmod;
    camera->scroll.y += (double)ymod;
}

void set_zoom(Camera *camera, Bool const out)
{
    camera->scale = out ? camera->scale/2 : camera->scale*2;
    if (camera->scale > MAX_ZOOM && !out)
        camera->scale = MAX_ZOOM;
    else if (camera->scale < MIN_ZOOM && out)
        camera->scale = MIN_ZOOM;
}

void reset_camera(Camera *camera)
{
    reset_coord(&camera->scroll);
    camera->scale = 1.0;
}

void center_camera(Camera *camera)
{
    camera->scroll.x = DEFAULT_CAM_X(*camera);
    camera->scroll.y = DEFAULT_CAM_Y(*camera);
}

/* Convert isometric coords to cartesian, taking acount of scale, and returns
 * cartesian Coord. */
static Coord isometric_to_cartesian(Coord const isometric, Camera const camera)
{
    Coord cartesian; init_coord(&cartesian);
    cartesian.x = isometric.y/TILES_HEIGHT(camera) + (isometric.x - SCREEN_WIDTH/2)/TILES_WIDTH(camera);
    cartesian.y = isometric.y/TILES_HEIGHT(camera) - (isometric.x - SCREEN_WIDTH/2)/TILES_WIDTH(camera);
    return cartesian;
}

Coord screen_to_coord(int const x, int const y, Camera const camera)
{
    Coord coord = int_to_coord(x, y);
    coord = isometric_to_cartesian(coord, camera);
    coord.x += camera.scroll.x; coord.y += camera.scroll.y; // scroll correction
    return coord;
}

/* Convert cartesian coords to isometric, taking acount of scale, and returns
 * isometric Coord. */
static Coord cartesian_to_isometric(Coord const cartesian, Camera const camera)
{
    Coord isometric; init_coord(&isometric);
    isometric.x = (cartesian.x - cartesian.y) * TILES_WIDTH(camera)/2 + SCREEN_WIDTH/2;
    isometric.y = (cartesian.x + cartesian.y) * TILES_HEIGHT(camera)/2;
    return isometric;
}

/* Convert cartesian coord to a SDL_Rect struct used in isometric graphic
 * representation, taking account of camera settings. */
static SDL_Rect coord_to_isosdlrect(Coord const coord, Camera const camera)
{

    Coord isometrified = coord;
    isometrified.x -= camera.scroll.x; isometrified.y -= camera.scroll.y; // scroll correction
    isometrified = cartesian_to_isometric(isometrified, camera);
    int x, y; coord_to_int(isometrified, &x, &y);

    SDL_Rect rect;
    set_image_position(&rect, x, y);
    set_image_size(&rect, TILES_WIDTH(camera), TILES_HEIGHT(camera));
    return rect;
}

/* This function is used to render tiles at their right position, depending on
 * where the camera is placed.
 * It iterates through all possible visible positions on screen because all
 * positions should have a tile. */
static void apply_tiles(
        SDL_Renderer *renderer,
        Asset const assets[0xFFFF],
        Map const map,
        Camera const camera,
        Bool const grid,
        int const level
        )
{
    int id = 0;
    SDL_Rect image_rect; init_sdl_rect(&image_rect);
    Coord positions; init_coord(&positions);
    Coord max_coord; init_coord(&max_coord);

    /* This defines the minimum and maximum position visible on screen.
     * The positions starts at -1 and goes to max to ensure that the tile on
     * the screen borders is rendered, even not completely */
    int minx = (int)(camera.scroll.x - 1.0);
    int maxx = (int)(camera.scroll.x + SCREEN_WIDTH/TILES_WIDTH(camera));
    int miny = (int)(camera.scroll.y - 1.0);
    int maxy = (int)(camera.scroll.y + SCREEN_HEIGHT/TILES_HEIGHT(camera));

    for (int y=miny;y<=maxy;y++)
        for (int x=minx;x<=maxx;x++)
        {
            positions = int_to_coord(x, y);
            if (!is_out_of_map(positions, map))
            {
                image_rect = coord_to_isosdlrect(positions, camera);

                /* Here we draw a grid if the corresponding bool is TRUE.
                 * This is mainly used in the map creator.
                 * The asset is a white empty tile with black borders. */
                if (level == 0 && grid)
                    SDL_RenderCopy(renderer, assets[EMPTY_TILE_ID].image->texture, NULL, &image_rect);

                id = map.tiles[level][y][x];
                if (id != 0) // A null ID means no tile is applied
                {
                    image_rect.y -= level * TILES_HEIGHT(camera); // level X offset
                    SDL_RenderCopy(renderer, assets[id].image->texture, NULL, &image_rect);
                }
            }
        }
}

/* This function determines if a position is visible on screen and returns TRUE
 * if so, even partially. */
static Bool is_in_sight(Coord const tile_position, Camera const camera)
{
    /* This defines the minimum and maximum position visible on screen.
     * The positions starts at -1 and goes to max to ensure that the tiles on
     * the screen borders are rendered, even not completely */
    Coord min_sight; init_coord(&min_sight);
    min_sight.x = camera.scroll.x - 1.0;
    min_sight.y = camera.scroll.y - 1.0;

    Coord max_sight; init_coord(&max_sight);
    max_sight.x = camera.scroll.x + SCREEN_WIDTH/TILES_WIDTH(camera);
    max_sight.y = camera.scroll.y + SCREEN_HEIGHT/TILES_HEIGHT(camera);

    if (
            (tile_position.x >= min_sight.x && tile_position.y >= min_sight.y) &&
            (tile_position.x <= max_sight.x && tile_position.y <= max_sight.y)
       )
        return TRUE;

    return FALSE;
}

/* This function specifically render characters.
 * It loops through all th assets ID corresponding to characters and checks if
 * their position is on screen, and if so, renders their image. */
static void apply_characters(
        SDL_Renderer *renderer,
        Asset const assets[0xFFFF],
        Map const map,
        Camera const camera
        )
{
    int current_frame;
    SDL_Rect image_rect; init_sdl_rect(&image_rect);
    Coord char_position; init_coord(&char_position);
    State state = MOVE;
    Cardinals direction = N;

    for (int id=FIRST_CHAR_ID;id<=LAST_CHAR_ID;id++)
    {
        char_position = assets[id].movement->position;
        if (
                !is_out_of_map(char_position, map) &&
                is_in_sight(char_position, camera)
            )
        {
            image_rect = coord_to_isosdlrect(char_position, camera);
            // This is to center characters on tile
            image_rect.y -= TILES_HEIGHT(camera);
            /* FRAMES dim are /2 because the character image is too big for the
             * tiles. */
            image_rect.w = scale_size(FRAMES_WIDTH/2, camera);
            image_rect.h = scale_size(FRAMES_HEIGHT/2, camera);
            direction = assets[id].movement->direction;
            current_frame = assets[id].animation->current_frame;
            SDL_RenderCopy(
                    renderer,
                    assets[id].image->texture,
                    &assets[id].animation->frames[direction][state][current_frame],
                    &image_rect
                    );
        }
    }
}

void render_screen(
        SDL_Renderer *renderer,
        Asset const assets[0xFFFF],
        int const mouse_type,
        Camera const camera,
        Map const map,
        Bool const paused,
        Bool const grid
        )
{
    Coord position; init_coord(&position);
    SDL_RenderClear(renderer); // erase the screen first

    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF); // RGBA, white
    SDL_RenderDrawRect(renderer, NULL);

    /* Here we apply all the assets.
     * First, we apply the level 0 of map, aka the ground. This is specifically
     * done *before* the rest because of the isometric point of view: The
     * characters must be above the ground, but behind all tiles that are above
     * level 0 (all characters are at level 0 max).
     * After the level 0 and characters are applied, all remaining levels are
     * done in a simple loop. */
    apply_tiles(renderer, assets, map, camera, grid, 0); // level 0
    if(!grid) // grid is only used for map making
        apply_characters(renderer, assets, map, camera);

    for (int level=1;level<MAX_LEVELS;level++)
        apply_tiles(renderer, assets, map, camera, grid, level);

    /* Apply the pause layer above everything if game is paused.
     * This is useful for pause layers that are transparent. */
    if (paused)
    {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 50); // RGBA, black w/ transparency
        SDL_RenderFillRect(renderer, NULL);
        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF); // RGBA, white
    }

    // Render mouse pointer above everything.
    SDL_RenderCopy(renderer, assets[mouse_type].image->texture, NULL, &assets[mouse_type].image->sdlrect);

    // show destination for characters
    for (int i=FIRST_CHAR_ID;i<=LAST_CHAR_ID;i++)
        if (assets[i].movement->moving)
        {
            position = assets[i].movement->path[0];
            SDL_Rect destination = coord_to_isosdlrect(position, camera);
            SDL_RenderCopy(renderer, assets[VALID].image->texture, NULL, &destination);
        }
}
