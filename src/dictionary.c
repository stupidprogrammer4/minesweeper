#include <time.h>

#include "dictionary.h"

#define DICT_DEF_CAPA 32
#define MOD(index, capa) ((index) & (capa - 1))
#define NEXT_PROB(index, dict) MOD(index + 1, dict->capa)
#define MAX(a, b) ((a) > (b) ? (a) : (b))

struct entry {
    void (*clean)(void *);
    char *key;
    void *val;
};

static int max_psl;

static 
usize djb2_hash(char *key);

static 
void _entry_swap(struct entry **ent1, struct entry **ent2);

static inline 
struct entry *_init_entry(char *key, void *val, usize val_size, void (*clean)(void *));

static inline 
void _entry_clean(struct entry **ent);

static inline 
usize _calc_index(dictionary *dict, char *key);

static inline 
struct entry *_get_entry(struct entry **table, char *key,
                                       usize index, usize capa);

static inline 
usize _calc_psl(dictionary *dict, char *key, usize loc);

static inline 
bool _table_insert(dictionary *dict, struct entry **table,
                                 struct entry *ent);

static inline
void rehash(dictionary *dict, usize new_capa);

static 
usize djb2_hash(char *key) {
    usize hash = 0x1505, i=0;
    while (key[i] != '\0') {
        hash = ((hash << 5) + hash) + key[i++];
    }
    return hash;
}

static 
void _entry_swap(struct entry **ent1, struct entry **ent2) {
    char *tmp_key = (*ent1)->key;
    (*ent1)->key = (*ent2)->key;
    (*ent2)->key = tmp_key;

    void *tmp_val = (*ent1)->val;
    (*ent1)->val = (*ent2)->val;
    (*ent2)->val = tmp_val;
}

static inline 
struct entry *_init_entry(char *key, void *val, usize val_size, void (*clean)(void *)) {
    struct entry *ent = malloc(sizeof(struct entry));
    ent->key = malloc(strlen(key) + 1);
    strcpy(ent->key, key);
    ent->val = malloc(val_size);
    memcpy(ent->val, val, val_size);
    ent->clean = clean;
    return ent;
}

static inline 
void _entry_clean(struct entry **ent) {
    free((*ent)->key);
    (*ent)->clean((*ent)->val);
    free(*ent);
    *ent = NULL;
}

static inline 
usize _calc_index(dictionary *dict, char *key) {
    usize h = dict->hash(key);
    return MOD(h, dict->capa);
}

static inline 
struct entry *_get_entry(struct entry **table, char *key,
                                       usize index, usize capa) {
    for (int i = 0; i <= max_psl; i++) {
        usize loc = MOD(index + i, capa);
        if (!table[loc])
            continue;
        if (strcmp(table[loc]->key, key) == 0)
            return table[loc];
    }
  return NULL;
}

static inline 
usize _calc_psl(dictionary *dict, char *key, usize loc) {
    usize h = _calc_index(dict, key);
    return (loc >= h) ? (loc - h) : (dict->capa + loc - h);
}

static inline 
bool _table_insert(dictionary *dict, struct entry **table,
                                 struct entry *ent) {
    usize index = _calc_index(dict, ent->key), psl = 0;
    struct entry *check_ent = _get_entry(table, ent->key, index, dict->capa);
    if (check_ent) {
        _entry_clean(&ent);
        return false;
    }
    while (1) {
        if (!table[index]) {
            table[index] = ent;
            dict->size++;
            return true;
        }
        usize p = _calc_psl(dict, table[index]->key, index);
        if (p >= psl) {
            index = NEXT_PROB(index, dict);
            psl++;
            max_psl = MAX(psl, max_psl);
        } 
        else {
            _entry_swap(&ent, &table[index]);
            index = NEXT_PROB(index, dict);
            psl = _calc_psl(dict, ent->key, index);
            max_psl = MAX(max_psl, psl);
        }
    }
}

static inline
void rehash(dictionary *dict, usize new_capa) {
    usize old_capa = dict->capa;
    dict->capa = new_capa;
    struct entry **new_table = malloc(sizeof(struct entry *) * dict->capa);

    for (int i=0; i<dict->capa; i++) new_table[i] = NULL;

    max_psl = 0;
    for (int i = 0; i < old_capa; i++)
        if (dict->table[i])
            _table_insert(dict, new_table, dict->table[i]);

    free(dict->table);
    dict->table = new_table;
}

dictionary *def_init_dict(usize val_size, void (*clean)(void *)) {
    dictionary *dict = malloc(sizeof(dictionary));
    dict->val_size = val_size;
    dict->hash = djb2_hash;
    dict->clean = clean;
    dict->capa = DICT_DEF_CAPA;
    dict->size = 0;
    dict->table = malloc(sizeof(struct entry *) * dict->capa);
    for (int i=0; i<dict->capa; i++) dict->table[i] = NULL;
    return dict;
}

dictionary *init_dict(usize val_size, usize (*hash)(char *),
                      void (*clean)(void *)) {
    dictionary *dict = malloc(sizeof(dictionary));
    dict->val_size = val_size;
    dict->hash = hash;
    dict->clean = clean;
    dict->capa = DICT_DEF_CAPA;
    dict->size = 0;
    dict->table = malloc(sizeof(struct entry *) * dict->capa);
    for (int i=0; i<dict->capa; i++) dict->table[i] = NULL;
    return dict;
}

bool dict_insert(dictionary *dict, char *key, void *val) {
    if (key == NULL || *key == '\0') {
        return false;
    }
    if (dict->size >= dict->capa/2) {
        usize new_capa = dict->capa*2;
        rehash(dict, new_capa);
    }
    struct entry *ent = _init_entry(key, val, dict->val_size, dict->clean);
    return _table_insert(dict, dict->table, ent);
}

void *dict_get(dictionary *dict, char *key) {
    usize index = _calc_index(dict, key);
    struct entry *ent = _get_entry(dict->table, key, index, dict->capa);
    if (ent) 
        return ent->val;
    return NULL;
}

bool dict_remove(dictionary *dict, char *key) {
    if (dict->size < dict->capa/8 && dict->capa/2 >= DICT_DEF_CAPA) {
        rehash(dict, dict->capa/2);
    }
    usize index = _calc_index(dict, key);
    for (int i=0; i<=max_psl; i++) {
        usize loc = MOD(index + i, dict->capa);
        if (dict->table[loc] && strcmp(dict->table[loc]->key, key) == 0) {
            _entry_clean(&dict->table[loc]);
            dict->size--;
            return true;
        }
    }
    return false;
}

bool dict_has_key(dictionary *dict, char *key) {
    usize index = _calc_index(dict, key);
    struct entry *ent = _get_entry(dict->table, key, index, dict->capa);
    if (ent) 
        return true;
    return false;
}

void dict_clean(dictionary *dict) {
    if (dict == NULL) 
        return;
    for (int i=0; i<dict->capa; i++)
        if (dict->table[i])
            _entry_clean(&dict->table[i]);
    free(dict->table);
    free(dict);
}
