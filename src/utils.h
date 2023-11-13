#ifndef __UTILS_
#define __UTILS__

#include "common.h"


SDL_Texture *create_text_texture(SDL_Renderer *ren, const char *text, 
    TTF_Font *font, SDL_Color fg);
void draw_text(SDL_Renderer *ren, TTF_Font *font, const char *text, SDL_Rect pos, SDL_Color fg);
SDL_Texture *load_img_texture(SDL_Renderer *ren, const char *path);
void destroy_texture_helper(void *texture);


#endif //__UTILS__