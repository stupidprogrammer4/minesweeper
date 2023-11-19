#include <dirent.h>

#include "game.h"
#include "manager.h"
#include "utils.h"

static inline 
void reveal(game *gm, int x, int y);

static inline
void change_image(game *gm, cell **c, char *image_key);

static inline
void flag(game *gm, int x, int y);

static
pair *construct_pair(int first, int second);

static
int cmp_pair(void *p1, void *p2);

static
cell *init_cell(game *gm, int x, int y, int w, int h);

static inline
void build2dgrid(game *gm, int row, int col, int tile_size);

static 
char *get_key(int val);

static inline 
void set_random_mine(game *gm, int mine_count);

static inline
void set_neigbour_count(game *gm, int x, int y);

static 
char *get_key(int val) {
    if (val < 0)
        return "mine";
    int n = val, i=0;
    do {
        i++;
        n /= 10;
    } while (n);

    char *result = malloc(i+1);
    sprintf(result, "%d", val);
    result[i] = '\0';

    return result;
}

static
pair *construct_pair(int first, int second) {
    pair *p = malloc(sizeof(pair));
    p->first = first;
    p->second = second;
    return p;
}

static
int cmp_pair(void *pp1, void *pp2) {
    pair **ptr1 = pp1, **ptr2 = pp2;
    pair *p1 = *ptr1, *p2 = *ptr2;
    if (p1->first == p2->first && p1->second == p2->second) 
        return 0;
    if (p1->first == p2->first)
        return p1->second - p2->second;
    return p1->first - p2->first;
}

static inline
void change_image(game *gm, cell **c, char *image_key) {
    SDL_Texture *image = DICT_GET_VAL(SDL_Texture *, gm->mn->textures, image_key);
    (*c)->image = image;
}

static inline
void flag(game *gm, int x, int y) {
    if (gm->grid[y][x]->revealed)
        return;
    if (gm->grid[y][x]->flagged) {
        change_image(gm, &gm->grid[y][x], "default");
        if (gm->grid[y][x]->is_mine)
            gm->mine_rem++;
        gm->grid[y][x]->flagged = false;
        gm->flags--;
    }
    else {
        change_image(gm, &gm->grid[y][x], "flag");
        if (gm->grid[y][x]->is_mine)
            gm->mine_rem--;
        gm->grid[y][x]->flagged = true;
        gm->flags++;
    }
    sprintf(gm->rem_flags, "%d/%d", gm->flags, gm->mine_count);
    gm->win = gm->mine_rem == 0;
}

static inline 
void reveal(game *gm, int x, int y) {
    if (gm->grid[y][x]->revealed || gm->grid[y][x]->flagged)
        return;
    
    int cnt = gm->grid[y][x]->neighbour_count;
    char *key = get_key(cnt);
    change_image(gm, &gm->grid[y][x], key);
    gm->grid[y][x]->revealed = true;
    if (gm->grid[y][x]->is_mine) {
        gm->over = true;
        for (int i=0; i<gm->row; i++)
            for (int j=0; j<gm->col; j++)
                if (gm->grid[i][j]->is_mine)
                    reveal(gm, j, i);
    }
    if (!cnt) 
        for (int i=-1; i<=1; i++)
            for (int j=-1; j<=1; j++)
                if ((y+i >= 0) && (y+i < gm->row) && (x+j >= 0) && (x+j < gm->col))
                    reveal(gm, x+j, y+i);
}

static inline
void set_neigbour_count(game *gm, int x, int y) {
    if (gm->grid[y][x]->is_mine) {
        gm->grid[y][x]->neighbour_count = -1;
        return;
    }
    for (int i=-1; i<=1; i++)
        for (int j=-1; j<=1; j++)
            if ((y+i >= 0) && (y+i < gm->row) && (x+j >= 0) && (x+j < gm->col)) 
                if (gm->grid[y+i][x+j]->is_mine)
                    gm->grid[y][x]->neighbour_count++;
}


static inline
void build2dgrid(game *gm, int row, int col, int tile_size) {
    gm->grid = malloc(sizeof(cell **)*row);
    for (int i=0; i<row; i++) {
        gm->grid[i] = malloc(sizeof(cell *)*col);
        for (int j=0; j<col; j++) {
            gm->grid[i][j] = init_cell(gm, gm->st_col+tile_size*j, gm->st_row+tile_size*i, tile_size, tile_size);
            pair *p = construct_pair(j, i);
            ls_append(gm->tiles, &p);
        }
    }
    
}

static inline 
void set_random_mine(game *gm, int mine_count) {
    srand(time(0));
    for (int i=0; i<mine_count; i++) {
        int rand_cell = rand() % gm->tiles->size;
        pair *p = LS_INDEX_SEARCH(pair *, gm->tiles, rand_cell);
        gm->grid[p->second][p->first]->is_mine = true;
        ls_erase(gm->tiles, rand_cell);
    }
}

static
cell *init_cell(game *gm, int x, int y, int w, int h) {
    cell *c = malloc(sizeof(cell));
    c->rect.x = x;
    c->rect.y = y;
    c->rect.w = w;
    c->rect.h = h;
    c->neighbour_count = 0;
    c->is_mine = false;
    c->revealed = false;
    c->flagged = false;
    c->image = DICT_GET_VAL(SDL_Texture *, gm->mn->textures, "default");
    return c;
}

game *init_game(manager *mn)
{
    game *gm = malloc(sizeof(game));
    gm->tiles = init_list(sizeof(pair *), cmp_pair, NULL);
    gm->grid = NULL;
    gm->mn = mn;
    gm->clock = mn->clock;
    return gm;
}

void set_game(game *gm, int row, int col, 
        int st_row, int st_col, int tile_size, int mine_count) 
{
    if (gm->grid && gm->tiles->size)
        clean_game_res(gm);
    gm->row = row;
    gm->col = col;
    gm->mine_count = mine_count;
    gm->over = false;
    gm->win = false;
    gm->st_row = st_row;
    gm->st_col = st_col;
    gm->mine_rem = mine_count;
    gm->flags = 0;
    build2dgrid(gm, row, col, tile_size);
    set_random_mine(gm, mine_count);
    for (int i=0; i<row; i++)
        for (int j=0; j<col; j++)
            set_neigbour_count(gm, j, i);
}

void draw_grid(game *gm) {
    for (int i=0; i<gm->row; i++)
        for (int j=0; j<gm->col; j++)
            SDL_RenderCopy(gm->mn->ren, gm->grid[i][j]->image, NULL, &gm->grid[i][j]->rect);
}

void reset(game *gm) {
    gm->over = false;
    gm->win = false;
    gm->flags = 0;
    gm->mine_rem = gm->mine_count;
    reset_clock(gm->clock);
    ls_clear(gm->tiles);

    for (int i=0; i<gm->row; i++) {
        for (int j=0; j<gm->col; j++) {
            gm->grid[i][j]->image = DICT_GET_VAL(SDL_Texture *, gm->mn->textures, "default");
            gm->grid[i][j]->revealed = gm->grid[i][j]->flagged = 0;
            gm->grid[i][j]->neighbour_count = 0;
            gm->grid[i][j]->is_mine = 0;
            pair *p = construct_pair(j, i);
            ls_append(gm->tiles, &p);
        }
    }

    set_random_mine(gm, gm->mine_count);
    for (int i=0; i<gm->row; i++)
        for (int j=0; j<gm->col; j++)
            set_neigbour_count(gm, j, i);
}

void update_game(game *gm, int mx, int my, bool rightclick) {
    if (gm->over || gm->win)
        return;
    for (int i=0; i<gm->row; i++) {
        for (int j=0; j<gm->col; j++){
            SDL_Rect r = gm->grid[i][j]->rect;
            if (mx >= r.x && mx <= r.x + r.w && my >= r.y && my <= r.y + r.h) {
                if (!rightclick)
                    reveal(gm, j, i);
                else
                    flag(gm, j, i);
                if (gm->clock->stopped)
                    start_clock(gm->clock);
                return;
            }
        }
    }  
}

void clean_game_res(game *gm) {
    ls_clear(gm->tiles);
    if (gm->grid) {
        for (int i=0; i<gm->row; i++)
            for (int j=0; j<gm->col; j++)
                if (gm->grid[i][j])
                    free(gm->grid[i][j]);
        
        for (int i=0; i<gm->row; i++)
            free(gm->grid[i]);

        free(gm->grid);

        gm->grid = NULL;
    }
}


void clean_game(game *gm) {
    clean_game_res(gm);
    ls_clean(gm->tiles);
    free(gm);
}