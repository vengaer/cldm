#include "cldm_hash.h"
#include "cldm_macro.h"

#include <stdbool.h>

#define CLDM_FNV_OFFSET_BASIS 0xcbf29ce484222325llu
#define CLDM_FNV_PRIME 0x100000001b3llu

void cldm_ht_free(struct cldm_ht *ht);

static inline size_t cldm_hash_entry(struct cldm_ht_entry const *entry) {
    return cldm_hash_fnv1a(entry->key, entry->size);
}

static inline size_t cldm_ht_sizeup(size_t current) {
    return (current << 1) | 0x1;
}

static inline struct cldm_ht_entry **cldm_ht_storage(struct cldm_ht *ht) {
    return ht->capacity > cldm_ht_static_capacity() ? ht->t_un.dyn : ht->t_un.stat;
}

static inline bool cldm_ht_should_expand(struct cldm_ht const *ht) {
    return ht->size + 1 > (ht->capacity >> 1);
}

static struct cldm_ht_entry **cldm_ht_probe(struct cldm_ht *restrict ht, struct cldm_ht_entry *restrict entry) {
    struct cldm_ht_entry **entries;
    size_t hash;
    size_t idx;

    entries = cldm_ht_storage(ht);

    hash = cldm_hash_entry(entry);
    idx = hash % ht->capacity;

    for(; entries[idx]; idx = (idx + 1) % ht->capacity) {
        if(entries[idx]->size == entry->size && memcmp(entries[idx]->key, entry->key, entry->size) == 0) {
            return &entries[idx];
        }
    }

    return &entries[idx];
}

static bool cldm_ht_expand(struct cldm_ht *ht) {
    struct cldm_ht_entry **data;
    size_t new_capacity;
    size_t prev_capacity;
    bool pdyn;
    union {
        struct cldm_ht_entry *stat[cldm_ht_static_capacity()];
        struct cldm_ht_entry **dyn;
    } t_un;

    new_capacity = cldm_ht_sizeup(ht->capacity);
    data = calloc(new_capacity, sizeof(*data));

    if(!data) {
        return false;
    }

    if(ht->capacity == cldm_ht_static_capacity()) {
        memcpy(t_un.stat, ht->t_un.stat, sizeof(t_un.stat));
        pdyn = false;
    }
    else {
        t_un.dyn = ht->t_un.dyn;
        pdyn = true;
    }
    prev_capacity = ht->capacity;

    ht->t_un.dyn = data;
    ht->capacity = new_capacity;
    ht->size = 0u;


    if(pdyn) {
        for(unsigned i = 0; i < prev_capacity; i++) {
            if(t_un.dyn[i]) {
                cldm_ht_insert(ht, t_un.dyn[i]);
            }
        }
        free(t_un.dyn);
    }
    else {
        for(unsigned i = 0; i < prev_capacity; i++) {
            if(t_un.stat[i]) {
                cldm_ht_insert(ht, t_un.stat[i]);
            }
        }
    }

    return true;
}

size_t cldm_hash_fnv1a(unsigned char const *data, size_t size) {
    size_t hash = CLDM_FNV_OFFSET_BASIS;
    unsigned char const *iter;

    cldm_for_each(iter, data, size) {
        hash ^= (size_t)*iter;
        hash *= CLDM_FNV_PRIME;
    }

    return hash;
}

struct cldm_ht_entry *cldm_ht_find(struct cldm_ht *restrict ht, struct cldm_ht_entry *restrict entry) {
    struct cldm_ht_entry **found;

    found = cldm_ht_probe(ht, entry);
    return (*found)->key ? *found : 0;
}

struct cldm_ht_entry *cldm_ht_insert(struct cldm_ht *restrict ht, struct cldm_ht_entry *restrict entry) {
    struct cldm_ht_entry **slot;

    if(cldm_ht_should_expand(ht)) {
        if(!cldm_ht_expand(ht)) {
            return 0;
        }
    }

    slot = cldm_ht_probe(ht, entry);
    *slot = entry;
    ++ht->size;
    return *slot;
}
