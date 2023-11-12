#include <dirent.h>

#include "manager.h"
#include "game.h"
#include "common.h"
#include "clock.h"


SDL_Rect cl_rect = {10, 15, 70, 20};
SDL_Color cl_fg = {0, 0, 0, 255};
bool run = 1, gamover=0;


static 
void close_font_helper(void *ttf_font);

static 
void destroy_texture_helper(void *texture);

static inline
char *font_key(const char *font_name, usize size);

static inline
char *filename(const char *fullname);

static inline 
char *f_basename(const char *path);

static inline
SDL_Texture *load_img_texture(SDL_Renderer *ren, const char *path);

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
SDL_Texture *load_img_texture(SDL_Renderer *ren, const char *path) {
    SDL_Surface *img_surface = IMG_Load(path);
    if (!img_surface) {
        fprintf(stderr, "IMG_Load() failed: %s\n", SDL_GetError());
    }
    SDL_Texture *img_texture = SDL_CreateTextureFromSurface(ren, img_surface);
    if (!img_texture) {
        fprintf(stderr, "IMG_CreateTextureFromSurface() failed: %s\n", SDL_GetError());
        exit(1);
    }
    SDL_FreeSurface(img_surface);
    return img_texture;
}

static inline
char *get_fullpath(const char *path, const char *name) {
    usize n = strlen(path), m = strlen(name);
    char *result = malloc(n+m+2);
    sprintf(result, "%s/%s", path, name);
    return result;
}

static
void destroy_texture_helper(void *texture) {
    SDL_Texture **ptr = texture;
    SDL_DestroyTexture(*ptr);
}

manager *init_manager(const char *title, int screen_width, int screen_height, 
        u32 sdl_flags ,u32 win_flags, u32 ren_flags, u32 img_flags, SDL_Color back_color)
{
    manager *mn = malloc(sizeof(manager));
    mn->screen_height = screen_height;
    mn->screen_width = screen_width;
    mn->back_color = back_color;
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

void build_game(manager *mn, int row, int col, 
        int tile_size, int mine_count) 
{
    int st_row = mn->screen_height - (tile_size*row);
    int st_col = mn->screen_width - (tile_size*col);
    mn->gm = init_game(mn, row, col, st_row, 
            st_col, tile_size, mine_count);
}

void start_game(manager *mn) {
    mn->cl = init_clock(mn, mn->font, cl_rect, cl_fg);
    int mx, my;
    SDL_Event e;
    

    bool clicked, flag;
    while (run) {
        SDL_SetRenderDrawColor(mn->ren, mn->back_color.r, mn->back_color.g, mn->back_color.b, mn->back_color.a);
        SDL_RenderClear(mn->ren);
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                run = false;
                mn->gm->isover = true;
                break;
            }
            else if (e.type == SDL_MOUSEBUTTONDOWN) {
                mx = e.button.x;
                my = e.button.y;
                clicked = true;
                flag = e.button.button == SDL_BUTTON_RIGHT;
            }
        }
        if (clicked) {
            update_game(mn->gm, mx, my, flag);
            gamover = mn->gm->isover;
            clicked = false;
            flag = false;
        }
        if (gamover)
            stop_clock(mn->cl);
        show_clock(mn->cl);
        draw_grid(mn->gm);
        SDL_RenderPresent(mn->ren);
    }
    destroy_clock(mn->cl);
    clean_game(mn->gm);
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
    TTF_CloseFont(mn->font);
    SDL_DestroyRenderer(mn->ren);
    SDL_DestroyWindow(mn->win);
    SDL_Quit();
    free(mn);
}