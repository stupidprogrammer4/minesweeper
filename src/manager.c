#include <dirent.h>

#include "manager.h"
#include "game.h"
#include "common.h"
#include "utils.h"

#define DEF_BTN_SW 120
#define DEF_BTN_SH 50

SDL_Rect cl_rect = {950, 15, 70, 20};
SDL_Color black = {0, 0, 0, 255};
SDL_Color white = {255, 255, 255, 255};
SDL_Color red = {255, 0, 0, 255};
SDL_Color blue = {0, 0, 255, 255};
SDL_Color green = {0, 255, 0, 255};
SDL_Color def_btn = {0, 128, 255, 255};

ui_button *menu_buttons[10], *game_buttons[10]; 


bool gamover=0;


static inline
char *font_key(const char *font_name, usize size);

static inline
char *filename(const char *fullname);

static inline 
char *f_basename(const char *path);


static inline
char *get_fullpath(const char *path, const char *name);

static inline
char *font_key(const char *font_name, usize size) {
    usize n = size, i = 0, m = strlen(font_name);
    do {
        i++;
        n /= 10;
    } while (n);

    char *key = malloc(m+i+1);
    sprintf(key, "%s%ld", font_name, size);

    return key;
}

static inline 
char *f_basename(const char *path) {
    char *res = strrchr(path, '/');
    return res+1;
    
}

static inline
char *filename(const char *fullname){
    usize i=0;
    while (fullname[i] != '\0' && fullname[i] != '.') i++;

    char *f_name = malloc(i+1);
    strncpy(f_name, fullname, i);
    f_name[i] = '\0';
    return f_name;
}


static inline
char *get_fullpath(const char *path, const char *name) {
    usize n = strlen(path), m = strlen(name);
    char *result = malloc(n+m+2);
    sprintf(result, "%s/%s", path, name);
    return result;
}


manager *init_manager(const char *title, int screen_width, int screen_height, 
        u32 sdl_flags ,u32 win_flags, u32 ren_flags, u32 img_flags, SDL_Color back_color)
{
    manager *mn = malloc(sizeof(manager));
    mn->screen_height = screen_height;
    mn->screen_width = screen_width;
    mn->back_color = back_color;
    mn->state = 0;
    mn->run = 1;
    mn->textures = def_init_dict(sizeof(SDL_Texture *), destroy_texture_helper);
    if (SDL_Init(sdl_flags) < 0) {
        fprintf(stderr, "SDL_Init() failed: %s\n", SDL_GetError());
        exit(1);
    }

    if (TTF_Init() < 0) {
        fprintf(stderr, "TTF_Init() failed: %s\n", SDL_GetError());
        exit(1);
    }

    if (IMG_Init(img_flags) < 0) {
        fprintf(stderr, "IMG_Init() failed: %s\n", SDL_GetError());
        exit(1);
    }

    mn->win = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, 
            SDL_WINDOWPOS_CENTERED, screen_width, screen_height, win_flags);
    
    if (!mn->win) {
        fprintf(stderr, "SDL_CreateWindow() failed: %s\n", SDL_GetError());
        exit(1);
    }

    mn->ren = SDL_CreateRenderer(mn->win, -1, ren_flags);

    if (!mn->ren) {
        fprintf(stderr, "SDL_CreateRenderer() failed: %s\n", SDL_GetError());
        exit(1);
    }

    return mn;
}

void build_menu_ui(manager *mn) {
    int i;
    char *items[] = {"10*10", "15*15", "20*20", "30*30", "load"};
    for (i=0; i<5; i++) {
        SDL_Texture *texture = create_text_texture(mn->ren, items[i], mn->font, mn->back_color);
        dict_insert(mn->textures, items[i], &texture);
    } 
    int w = (mn->screen_width-60)/2, h=(mn->screen_height-120)/2; 
    for (i=0; i<4; i++) {
        SDL_Rect rect = {20+((i%2)*w)+((i%2)*20), 10+((i/2)*h)+((i/2)*20), w, h};
        SDL_Texture *texture = DICT_GET_VAL(SDL_Texture *, mn->textures, items[i]);
        menu_buttons[i] = init_button(rect, mn->back_color, def_btn, texture, items[i], 100, 50);
    }
    //SDL_Texture *texture = DICT_GET_VAL(SDL_Texture *, mn->textures, "load");
    //SDL_Rect rect = {mn->screen_width/2-(DEF_BTN_SW/2), h+h+40, DEF_BTN_SW, DEF_BTN_SH};
    //menu_buttons[4] = init_button(rect, mn->back_color, def_btn, texture, "load", DEF_BTN_SW-40, DEF_BTN_SH-10);
}

void run_game(manager *mn) {
    SDL_Rect state = {900, mn->screen_height/2+10, 200, 100};
    bool clicked=0, rightclick=0;
    int mx, my;
    while (SDL_PollEvent(&mn->e)) {
        if (mn->e.type == SDL_QUIT) {
            mn->run = 0;
            break;
        }
        else if (mn->e.type == SDL_MOUSEBUTTONDOWN) {
            mx = mn->e.button.x; my = mn->e.button.y;
            clicked = 1;
            rightclick = mn->e.button.button == SDL_BUTTON_RIGHT;
        }
    }
    if (clicked)
        update_game(mn->gm, mx, my, rightclick);
    if (mn->gm->over) {
        stop_clock(mn->clock);
        SDL_RenderCopy(mn->ren, DICT_GET_VAL(SDL_Texture *, mn->textures, "YOU LOSE!"), NULL, &state);
    }
    draw_grid(mn->gm);
    show_clock(mn->clock);
}

void build_game_ui(manager *mn) {
    mn->clock = init_clock(mn, mn->font, cl_rect, black);
    SDL_Texture *losing = create_text_texture(mn->ren, "YOU LOSE!", mn->font, red);
    dict_insert(mn->textures, "YOU LOSE!", &losing);
}

void run_menu(manager *mn) {
    bool clicked=0;
    int mx, my;
    while (SDL_PollEvent(&mn->e)) {
        if (mn->e.type == SDL_QUIT) {
            mn->run = 0;
            break;
        }
        else if (mn->e.type == SDL_MOUSEBUTTONDOWN) {
            mx = mn->e.button.x; my = mn->e.button.y;
            clicked = mn->e.button.button == SDL_BUTTON_LEFT;
        }
    }
    int ind=-1;
    for (int i=0; i<4; i++) 
        show_button(menu_buttons[i], mn->ren);
    if (clicked)
        for (int i=0; i<4; i++)
            if (is_clicked(menu_buttons[i], mx, my)) {
                ind = i;
                mn->state = 1;
                break;
            }   
    switch (ind)
    {
    case 0:
        build_game(mn, 10, 10, mn->screen_height/10, 15);
        break;
    case 1:
        build_game(mn, 15, 15, mn->screen_height/15, 35);
        break;
    case 2:
        build_game(mn, 20, 20, mn->screen_height/20, 62);
        break;
    case 3:
        build_game(mn, 30, 30, mn->screen_height/30, 140);
    default:
        break;
    }
}

void build_game(manager *mn, int row, int col, 
        int tile_size, int mine_count) 
{
    mn->gm = init_game(mn, row, col, 0, 
            0, mn->clock, tile_size, mine_count);
}


void load_font(manager *mn, const char *path, usize size) {
    mn->font = TTF_OpenFont(path, size);
    if (!mn->font) {
        fprintf(stderr, "TTF_OpenFont() failed: %s\n", SDL_GetError());
        exit(1);
    }
}

void load_images_textures(manager *mn, const char *path) {
    DIR *dir;
	dir = opendir(path);
	struct dirent *ent = readdir(dir);	
	while (ent) {
		if (ent->d_type == 8) {
			char *name = filename(ent->d_name);
            char *fullpath = get_fullpath(path, ent->d_name);
            SDL_Texture *texture = load_img_texture(mn->ren, fullpath);
            dict_insert(mn->textures, name, &texture);
		}
		ent = readdir(dir);
	}
}

void clean_manager(manager *mn) {
    dict_clean(mn->textures);
    destroy_clock(mn->clock);
    clean_game(mn->gm);
    TTF_CloseFont(mn->font);
    SDL_DestroyRenderer(mn->ren);
    SDL_DestroyWindow(mn->win);
    SDL_Quit();
    free(mn);
}