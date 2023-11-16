#ifndef __UI__
#define __UI__

#include "common.h"

struct MANAGER_;

typedef struct BUTTON_ {
  SDL_Rect rect, trect;
  SDL_Color bg;
  SDL_Texture *texture;
  char *text;
  int tw, th;
} ui_button;

typedef struct CLOCK_ {
  SDL_Rect rect;
  SDL_Color fg;
  TTF_Font *font;
  SDL_TimerID timer;
  char time[100];
  u64 elapsed_sec;
  struct MANAGER_ *mn;
  bool stopped;

} ui_clock;

ui_button *init_button(SDL_Rect rect, SDL_Color bg,
                       SDL_Texture *texture, char *text, int tw, int th);
void show_button(ui_button *button, SDL_Renderer *ren);
bool is_clicked(ui_button *button, int mx, int my);
ui_clock *init_clock(struct MANAGER_ *mn, TTF_Font *font, SDL_Rect rect,
                     SDL_Color fg);
void start_clock(ui_clock *cl);
void show_clock(ui_clock *cl);
void stop_clock(ui_clock *cl);
void destroy_clock(ui_clock *cl);
void destroy_button(ui_button *button);

#endif //__UI__
