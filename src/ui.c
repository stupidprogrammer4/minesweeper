#include "ui.h"
#include "manager.h"
#include "utils.h"

#define SEC 1000

static
unsigned int clock_callback(unsigned int interval, void *param) {
    ui_clock *cl = param;
    cl->elapsed_sec++;
    u32 min = cl->elapsed_sec / (u32)60;
    u32 sec = cl->elapsed_sec % (u32)60;
    sprintf(cl->time, "%02d:%02d", min, sec);

    return interval;
}

ui_button *init_button(SDL_Rect rect, SDL_Color bg,
        SDL_Texture *texture, char *text, int tw, int th) 
{
    ui_button *button = malloc(sizeof(ui_button));
    button->bg = bg;
    button->rect = rect;
    button->texture = texture;
    usize n = strlen(text);
    button->text = malloc(n+1);
    strcpy(button->text, text);
    button->text[n] = '\0';
    button->trect.x = rect.x+(rect.w/2)-(tw/2);
    button->trect.y = rect.y+(rect.h/2)-(th/2);
    button->trect.w = tw;
    button->trect.h = th;

    return button;
}

bool is_clicked(ui_button *button, int mx, int my) {
    return mx >= button->rect.x && mx <= button->rect.x+button->rect.w && 
           my >= button->rect.y && my <= button->rect.y+button->rect.h;
}

void show_button(ui_button *button, SDL_Renderer *ren) {
    SDL_SetRenderDrawColor(ren, button->bg.r, button->bg.g, button->bg.b, button->bg.a);
    SDL_RenderFillRect(ren, &button->rect);
    SDL_RenderCopy(ren, button->texture, NULL, &button->trect);
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
    draw_text(cl->mn->ren, cl->mn->font, cl->time, cl->rect, cl->fg);
}

void stop_clock(ui_clock *cl) {
    if (!cl->stopped) {
        SDL_RemoveTimer(cl->timer);
        cl->stopped = true;
    }
}

void reset_clock(ui_clock *cl) {
    stop_clock(cl);
    cl->elapsed_sec = 0;
    sprintf(cl->time, "00:00");
}

void destroy_button(ui_button *button) {
	free(button);
}

void destroy_clock(ui_clock *cl) {
    if (!cl->stopped)
        SDL_RemoveTimer(cl->timer);
    free(cl);
}
