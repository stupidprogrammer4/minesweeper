#include "common.h"
#include "dictionary.h"
#include "list.h"
#include "manager.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 850
#define TILE_SIZE 50
#define ROW 16
#define COL 16
#define MINE_COUNT 40
#define DEF_FONT_SIZE 24

int main(int argc, char *argv[]) {
    SDL_Color fg = {102, 178, 255, 255};
    manager *mn = init_manager("minesweeper", SCREEN_WIDTH, SCREEN_HEIGHT, 
                        SDL_INIT_VIDEO, 0, 0, IMG_INIT_PNG, fg);
    load_images_textures(mn, "../res/gfx");
    load_font(mn, "../res/font/font.ttf", DEF_FONT_SIZE);
    build_game(mn, ROW, COL, TILE_SIZE, MINE_COUNT);
    start_game(mn);
    clean_manager(mn);
}
