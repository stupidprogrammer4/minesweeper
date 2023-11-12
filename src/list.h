#ifndef __LIST__
#define __LIST__

#include "common.h"

typedef struct {
    i32 (*cmp)(void *, void *);
    void (*clean)(void *);
    usize item_size;
    usize size;
    usize capacity;
    void **items;
} list;

list *init_list(usize, i32 (*cmp)(void *, void *),
                     void (*clean)(void *));
void ls_append(list *ls, void *item);
void ls_insert(list *ls, i32 index, void *item);
void ls_pop(list *ls);
void ls_erase(list *ls, i32 index);
i32 ls_find(list *ls, void *item);
void *ls_get_value(list *ls, i32 index);
void ls_clear(list *ls);
void ls_clean(list *ls);
void ls_sort(list *ls);

#define LS_INDEX_SEARCH(type, ls, idx) *((type *)ls_get_value(ls, idx))

#endif // __LIST__
