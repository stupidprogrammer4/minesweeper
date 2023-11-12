#include "clock.h"
#include "manager.h"

#define SEC 1000

static
SDL_Texture *create_text_texture(SDL_Renderer *ren, const char *text, 
    TTF_Font *font, SDL_Color fg);

static
void draw_text(manager *mn, const char *text, SDL_Rect pos, SDL_Color fg);


static
void draw_text(manager *mn, const char *text, SDL_Rect pos, SDL_Color fg) {
    SDL_Texture *texture = create_text_texture(mn->ren, text, mn->font, fg);
    SDL_RenderCopy(mn->ren, texture, NULL, &pos);
    SDL_DestroyTexture(texture);
}

static
SDL_Texture *create_text_texture(SDL_Renderer *ren, const char *text, TTF_Font *font, SDL_Color fg) {
    SDL_Surface *surface = TTF_RenderText_Solid(font, text, fg);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(ren, surface);
    SDL_FreeSurface(surface);
    return texture;
}

static
unsigned int clock_callback(unsigned int interval, void *param) {
    ui_clock *cl = param;
    cl->elapsed_sec++;
    u32 min = cl->elapsed_sec / (u32)60;
    u32 sec = cl->elapsed_sec % (u32)60;
    sprintf(cl->time, "%02d:%02d", min, sec);

    return interval;
}

ui_clock *init_clock(manager *mn, TTF_Font *font, SDL_Rect rect, SDL_Color fg) {
    ui_clock *cl = malloc(sizeof(ui_clock));
    cl->mn = mn;
    cl->font = font;
    cl->rect = rect;
    cl->fg = fg;
    sprintf(cl->time, "00:00");
    cl->elapsed_sec = 0;
    cl->stopped = true;
    return cl;
}

void start_clock(ui_clock *cl) {
    if (cl->stopped) {
        cl->timer = SDL_AddTimer(SEC, clock_callback, cl);
        cl->stopped = false;
    }
}

void show_clock(ui_clock *cl) {
    draw_text(cl->mn, cl->time, cl->rect, cl->fg);
}

void stop_clock(ui_clock *cl) {
    if (!cl->stopped) {
        SDL_RemoveTimer(cl->timer);
        cl->stopped = true;
    }
    
}

void destroy_clock(ui_clock *cl) {
    if (!cl->stopped)
        SDL_RemoveTimer(cl->timer);
    free(cl);
}