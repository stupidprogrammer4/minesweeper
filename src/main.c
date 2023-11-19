#include "common.h"
#include "dictionary.h"
#include "list.h"
#include "manager.h"
#include "game.h"

#define SCREEN_WIDTH 1120
#define SCREEN_HEIGHT 900
#define TILE_SIZE 50
#define ROW 16
#define COL 16
#define MINE_COUNT 40
#define DEF_FONT_SIZE 24


int main(int argc, char *argv[]) {
    SDL_Color bg = {102, 178, 255, 255};
    manager *mn = init_manager("minesweeper", SCREEN_WIDTH, SCREEN_HEIGHT, 
                        SDL_INIT_VIDEO, 0, 0, IMG_INIT_PNG, bg);
    load_images_textures(mn, "../res/gfx");
    load_font(mn, "../res/font/font.ttf", DEF_FONT_SIZE);
    build_menu_ui(mn); 
    build_game_ui(mn);
    mn->gm = init_game(mn);
    while (mn->run) {
        SDL_SetRenderDrawColor(mn->ren, mn->back_color.r, mn->back_color.g, mn->back_color.b, mn->back_color.a);
        SDL_RenderClear(mn->ren);
        if (mn->state == 0)
            run_menu(mn);
        else if (mn->state == 1)
            run_game(mn);
        SDL_RenderPresent(mn->ren);
    }
    clean_manager(mn);
}
