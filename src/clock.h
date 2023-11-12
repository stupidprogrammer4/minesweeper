#ifndef __CLOCK__
#define __CLOCK__

#include "common.h"

struct MANAGER_;

typedef struct CLOCK_
{
    SDL_Rect rect;
    SDL_Color fg;
    TTF_Font *font;
    SDL_TimerID timer;
    char time[100];
    u64 elapsed_sec;
    struct MANAGER_ *mn;
    bool stopped;

} ui_clock;

ui_clock *init_clock(struct MANAGER_ *mn, TTF_Font *font, SDL_Rect rect, SDL_Color fg);
void start_clock(ui_clock *cl);
void show_clock(ui_clock *cl);
void stop_clock(ui_clock *cl);
void destroy_clock(ui_clock *cl);

#endif //__CLOCK__