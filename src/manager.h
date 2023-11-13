#ifndef __MANAGER__
#define __MANAGER__

#include "common.h"
#include "dictionary.h"

struct GAME_;
struct CLOCK_;

typedef struct MANAGER_
{
    int screen_width;
    int screen_height;
    int state;
    bool run;
    TTF_Font *font;
    dictionary *textures;
    SDL_Color back_color;
    SDL_Window *win;
    SDL_Renderer *ren;
    SDL_Event e;
    struct GAME_ *gm;  
    struct CLOCK_ *clock;
} manager;

manager *init_manager(const char *title, int screen_width, int screen_height, 
        u32 sdl_flags ,u32 win_flags, u32 ren_flags, u32 img_flags, SDL_Color back_color);
void load_font(manager *mn, const char *path, usize size);
void load_images_textures(manager *mn, const char *path);
void build_game(manager *mn, int row, int col,
                 int tile_size, int mine_count);
void build_menu_ui(manager *mn);
void build_game_ui(manager *mn);
void run_game(manager *mn);
void run_menu(manager *mn);
void start_game(manager *mn);
void clean_manager(manager *mn);


#endif //__MANAGER__