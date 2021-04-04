#include "cldm_hash.h"
#include "cldm_macro.h"

#include <stdbool.h>

#define CLDM_FNV_OFFSET_BASIS 0xcbf29ce484222325llu
#define CLDM_FNV_PRIME 0x100000001b3llu

enum { CLDM_HT_POS_OPEN = 1 };

void cldm_ht_free(struct cldm_ht *ht);
size_t cldm_ht_size(struct cldm_ht const *ht);
size_t cldm_ht_capacity(struct cldm_ht const *ht);

static inline bool cldm_ht_slot_open(union cldm_ht_internal_entry const *entry) {
    return entry->status <= CLDM_HT_POS_OPEN;
}

static bool cldm_hash_is_prime(size_t num) {
    if(num == 2 || num == 3) {
        return true;
    }
    if((!num % 2) || !(num % 3)) {
        return false;
    }

    for(size_t div = 6u; div * div - 2 * div < num; div += 6u) {
        if(!(num % (div - 1)) || !(num % div + 1)) {
            return false;
        }
    }

    return true;
}

static inline size_t cldm_hash_entry(struct cldm_ht_entry const *entry) {
    return cldm_hash_fnv1a(entry->key, entry->size);
}

static size_t cldm_ht_sizeup(size_t current) {
    current = ((current << 1) | 0x1);
    while(!cldm_hash_is_prime(current)) {
        current += 2;
    }
    return current;
}

static inline union cldm_ht_internal_entry *cldm_ht_storage(struct cldm_ht *ht) {
    return ht->capacity > cldm_ht_static_capacity() ? ht->t_un.dyn : ht->t_un.stat;
}

static inline bool cldm_ht_should_expand(struct cldm_ht const *ht) {
    return ht->size + 1 > (ht->capacity >> 1);
}

static union cldm_ht_internal_entry *cldm_ht_probe(struct cldm_ht *restrict ht, struct cldm_ht_entry const *restrict entry, bool first_open) {
    union cldm_ht_internal_entry *entries;
    size_t idx;

    entries = cldm_ht_storage(ht);
    idx = cldm_hash_entry(entry) % ht->capacity;

    for(; entries[idx].status; idx = (idx + 1) % ht->capacity) {
        if(cldm_ht_slot_open(&entries[idx])) {
            if(first_open) {
                return &entries[idx];
            }
            continue;
        }
        if(entries[idx].ent->size == entry->size && memcmp(entries[idx].ent->key, entry->key, entry->size) == 0) {
            return &entries[idx];
        }
    }

    return &entries[idx];

}

static bool cldm_ht_expand(struct cldm_ht *ht) {
    union cldm_ht_internal_entry *data;
    size_t new_capacity;
    size_t prev_capacity;
    bool pdyn;
    union {
        union cldm_ht_internal_entry stat[cldm_ht_static_capacity()];
        union cldm_ht_internal_entry *dyn;
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
            if(t_un.dyn[i].status) {
                cldm_ht_insert(ht, t_un.dyn[i].ent);
            }
        }
        free(t_un.dyn);
    }
    else {
        for(unsigned i = 0; i < prev_capacity; i++) {
            if(t_un.stat[i].status) {
                cldm_ht_insert(ht, t_un.stat[i].ent);
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

struct cldm_ht_entry *cldm_ht_find(struct cldm_ht *restrict ht, struct cldm_ht_entry const *restrict entry) {
    union cldm_ht_internal_entry *found;

    found = cldm_ht_probe(ht, entry, false);
    return cldm_ht_slot_open(found) ? 0 : found->ent;
}

struct cldm_ht_entry *cldm_ht_insert(struct cldm_ht *restrict ht, struct cldm_ht_entry *restrict entry) {
    union cldm_ht_internal_entry *slot;

    if(cldm_ht_should_expand(ht)) {
        if(!cldm_ht_expand(ht)) {
            return 0;
        }
    }

    slot = cldm_ht_probe(ht, entry, true);
    slot->ent = entry;
    ++ht->size;
    return slot->ent;
}

struct cldm_ht_entry *cldm_ht_remove(struct cldm_ht *restrict ht, struct cldm_ht_entry const *restrict entry) {
    union cldm_ht_internal_entry *slot;
    struct cldm_ht_entry *ret;

    slot = cldm_ht_probe(ht, entry, false);
    ret = slot->ent;
    slot->status = CLDM_HT_POS_OPEN;
    --ht->size;

    return ret;
}
