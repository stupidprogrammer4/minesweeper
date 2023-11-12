#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"

#define SIZE sizeof(void *)

static inline 
void _ensure_capacity(list *ls);

static inline 
void _shift_right(list *ls, i32 index);

static inline 
void _shift_left(list *ls, i32 index);

static void 
_merge(list *ls, i32 begin, int mid, int end);

static void 
_merge_sort(list *ls, i32 begin, int end);

static inline 
void _ensure_capacity(list *ls) {
    if (ls->size >= ls->capacity) {
        ls->capacity *= 2;
        ls->items = realloc(ls->items, SIZE * ls->capacity);
    }
    if (ls->size < ls->capacity / 4) {
        ls->capacity /= 2;
        ls->items = realloc(ls->items, SIZE * ls->capacity);
    }
}

static inline 
void _shift_right(list *ls, i32 index) {
    for (i32 i = ls->size - 1; i >= index + 1; i--) {
        memcpy(ls->items[i], ls->items[i - 1], ls->item_size);
    }
}

static inline 
void _shift_left(list *ls, i32 index) {
    for (i32 i = index; i < ls->size; i++) {
        memcpy(ls->items[i], ls->items[i + 1], ls->item_size);
    }
}

static void 
_merge(list *ls, i32 begin, int mid, int end) {
    i32 leftsub_size = mid - begin + 1, rightsub_size = end - mid;

    void **leftsub = malloc(SIZE * leftsub_size),
        **rightsub = malloc(SIZE * rightsub_size);

    for (i32 i = 0; i < leftsub_size; i++) {
        leftsub[i] = malloc(ls->item_size);
        memcpy(leftsub[i], ls->items[i + begin], ls->item_size);
    }

    for (i32 i = 0; i < rightsub_size; i++) {
        rightsub[i] = malloc(ls->item_size);
        memcpy(rightsub[i], ls->items[i + mid + 1], ls->item_size);
    }

    i32 i = 0, j = 0, k = begin;
    while (i < leftsub_size && j < rightsub_size) {
        if (ls->cmp(leftsub[i], rightsub[j]) <= 0)
            memcpy(ls->items[k++], leftsub[i++], ls->item_size);
        else
            memcpy(ls->items[k++], rightsub[j++], ls->item_size);
    }

    while (i < leftsub_size)
        memcpy(ls->items[k++], leftsub[i++], ls->item_size);

    while (j < rightsub_size)
        memcpy(ls->items[k++], rightsub[j++], ls->item_size);

    for (i32 i = 0; i < leftsub_size; i++)
        ls->clean(leftsub[i]);

    for (i32 j = 0; j < rightsub_size; j++)
        ls->clean(rightsub[j]);

    free(leftsub);
    free(rightsub);
}

static void 
_merge_sort(list *ls, i32 begin, int end) {
    if (begin >= end)
        return;
    i32 mid = (begin + end) / 2;
    _merge_sort(ls, begin, mid);
    _merge_sort(ls, mid + 1, end);
    _merge(ls, begin, mid, end);
}

list *init_list(usize item_size, i32 (*cmp)(void *, void *),
                     void (*clean)(void *)) {
    list *ls = malloc(sizeof(list));
    ls->capacity = 1;
    ls->size = 0;
    ls->item_size = item_size;
    ls->items = malloc(SIZE * 1);
    ls->cmp = cmp;
    ls->clean = clean == NULL ? free : clean;
    return ls;
}

void ls_append(list *ls, void *item) {
    ls->size++;
    _ensure_capacity(ls);
    ls->items[ls->size - 1] = malloc(ls->item_size);
    memcpy(ls->items[ls->size - 1], item, ls->item_size);
}

void ls_pop(list *ls) {
    if (ls->size <= 0)
        return;
    ls->size--;
    _ensure_capacity(ls);
    ls->clean(ls->items[ls->size]);
}

void ls_insert(list *ls, i32 index, void *item) {
    if (index < 0 || index > ls->size) {
        perror("index out of range");
        exit(1);
    }
    if (index == ls->size) {
        ls_append(ls, item);
        return;
    }
    ls->size++;
    _ensure_capacity(ls);
    ls->items[ls->size - 1] = malloc(ls->item_size);
    _shift_right(ls, index);
    memcpy(ls->items[index], item, ls->item_size);
}

void ls_erase(list *ls, i32 index) {
    if (index < 0 || index >= ls->size) {
        perror("index out of range");
        exit(1);
    }
    ls->size--;
    _ensure_capacity(ls);
    _shift_left(ls, index);
    ls->clean(ls->items[ls->size]);
}

i32 ls_find(list *ls, void *item) {
    if (ls->cmp == NULL) {
        return -1;
    }
    for (i32 i = 0; i < ls->size; i++)
        if (ls->cmp(ls->items[i], item) == 0)
            return i;

    return -1;
}

void *ls_get_value(list *ls, i32 index) {
    if (index < 0 || index >= ls->size) {
        perror("index out of range");
        exit(1);
    }
    return ls->items[index];
}

void ls_sort(list *ls) {
    if (ls->cmp == NULL) {
        perror("compare function not found!");
      exit(1);
    }
    _merge_sort(ls, 0, ls->size - 1);
}

void ls_clear(list *ls) {
    usize n = ls->size;
    for (i32 i = 0; i < n; i++)
        ls_pop(ls);
}

void ls_clean(list *ls) {
    if (ls == NULL)
        return;
    ls_clear(ls);
    free(ls->items);
    free(ls);
}
