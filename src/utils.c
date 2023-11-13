#include "utils.h"


void draw_text(SDL_Renderer *ren, TTF_Font *font, const char *text, SDL_Rect pos, SDL_Color fg) {
    SDL_Texture *texture = create_text_texture(ren, text, font, fg);
    SDL_RenderCopy(ren, texture, NULL, &pos);
    SDL_DestroyTexture(texture);
}

SDL_Texture *create_text_texture(SDL_Renderer *ren, const char *text, TTF_Font *font, SDL_Color fg) {
    SDL_Surface *surface = TTF_RenderText_Solid(font, text, fg);
    if (!surface) {
        fprintf(stderr, "TTF_RenderText_Solid() failed: %s\n", SDL_GetError());
        exit(1);
    }
    SDL_Texture *texture = SDL_CreateTextureFromSurface(ren, surface);
    if (!texture) {
        fprintf(stderr, "IMG_CreateTextureFromSurface() failed: %s\n", SDL_GetError());
        exit(1);
    }
    SDL_FreeSurface(surface);
    return texture;
}

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

void destroy_texture_helper(void *texture) {
    SDL_Texture **ptr = texture;
    SDL_DestroyTexture(*ptr);
}