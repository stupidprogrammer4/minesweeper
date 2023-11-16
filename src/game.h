#ifndef __GAME__
#define __GAME__

#include "common.h"
#include "dictionary.h"
#include "ui.h"
#include "list.h"

struct MANAGER_;

typedef struct 
{
  int first;
  int second;
} pair;

typedef struct 
{
    SDL_Texture *image;
    SDL_Rect rect;
    bool is_mine;
    bool revealed;
    bool flagged;
    int neighbour_count; 
} cell;

typedef struct GAME_
{
    int st_row;
    int st_col;
    bool over;
    bool win;
    bool game_started;
    int mine_count;
    int mine_rem;
    int row;
    int col;
    int tile_size;
    int flags;
    cell ***grid;
    char rem_flags[100];
    ui_clock *clock;
    struct MANAGER_ *mn; 
    list *tiles;
} game;

game *init_game(struct MANAGER_ *mn, int row, int col, 
        int st_row, int st_col, ui_clock *clock, int tile_size, int mine_count);
void draw_grid(game *gm);
void reset(game *gm);
void update_game(game *gm, int mx, int my, bool rightclick);
void clean_game(game *gm);

#endif //__GAME__