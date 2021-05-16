#ifndef CLDM_HASH_H
#define CLDM_HASH_H

#include "cldm_macro.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

enum { CLDM_HASH_STATIC_SIZE = 31 };

struct cldm_ht_entry {
    void const *key;
    size_t size;
};

union cldm_ht_internal_entry {
    struct cldm_ht_entry ent;
    size_t status;
};

struct cldm_ht {
    union {
        union cldm_ht_internal_entry stat[CLDM_HASH_STATIC_SIZE];
        union cldm_ht_internal_entry *dyn;
    } t_un;
    size_t size;
    size_t capacity;
};

#define cldm_ht_mkentry_str(str)    \
    (struct cldm_ht_entry){ .key = str, .size = strlen(str) }

#define cldm_ht_mkentry(val)    \
    (struct cldm_ht_entry){ .key = &(val), .size = sizeof(val) }

#define cldm_ht_static_capacity()   \
    cldm_arrsize(((struct cldm_ht *)0)->t_un.stat)

#define cldm_ht_init()    \
    (struct cldm_ht){ .capacity = cldm_ht_static_capacity() }

size_t cldm_hash_fnv1a(unsigned char const *data, size_t size);

inline void cldm_ht_free(struct cldm_ht *ht) {
    if(ht->capacity > cldm_ht_static_capacity()) {
        free(ht->t_un.dyn);
    }
}

inline size_t cldm_ht_size(struct cldm_ht const *ht) {
    return ht->size;
}

inline size_t cldm_ht_capacity(struct cldm_ht const *ht) {
    return ht->capacity;
}

struct cldm_ht_entry *cldm_ht_find(struct cldm_ht *restrict ht, struct cldm_ht_entry const *restrict entry);
struct cldm_ht_entry *cldm_ht_insert(struct cldm_ht *restrict ht, struct cldm_ht_entry *restrict entry);
bool cldm_ht_remove(struct cldm_ht *restrict ht, struct cldm_ht_entry const *restrict entry);

#endif /* CLDM_HASH_H */
