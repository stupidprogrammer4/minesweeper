#ifndef __DICTIONARY__
#define __DICTIONARY__

#include "common.h"

struct entry;

typedef struct {
    usize capa;
    usize size;
    usize val_size;
    usize (*hash)(char *);
    void (*clean)(void *);
    struct entry **table;
} dictionary;

dictionary *def_init_dict(usize, void (*)(void *));
dictionary *init_dict(usize, usize (*)(char *), void (*)(void *));
bool dict_insert(dictionary *, char *, void *);
void *dict_get(dictionary *, char *);
bool dict_has_key(dictionary *, char *);
bool dict_remove(dictionary *, char *);
bool dict_has_key(dictionary *, char *);
void dict_clean(dictionary *);

#define DICT_GET_VAL(type, dict, key) *((type *)(dict_get(dict, key)))

#endif //__DICTIONARY__
