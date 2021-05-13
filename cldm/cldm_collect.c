#include "cldm_collect.h"
#include "cldm_hash.h"
#include "cldm_log.h"
#include "cldm_macro.h"
#include "cldm_test.h"
#include "cldm_testrec.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

ssize_t cldm_collect(struct cldm_rbtree *restrict tree, struct cldm_elfmap const *restrict map) {
    struct cldm_testrec *record;

    /* Look up tests in strtab section */
    for(size_t i = 0; i < map->strtab.size; i += strlen(map->strtab.addr + i) + 1) {
        /* Identify by test prefix */
        if(strncmp(cldm_str_expand(cldm_testrec_prefix), map->strtab.addr + i, sizeof(cldm_str_expand(cldm_testrec_prefix)) - 1) == 0) {
            record = cldm_elf_testrec(map, map->strtab.addr + i);
            if(!record) {
                cldm_err("Could not map test record for %s", map->strtab.addr + i + sizeof(cldm_str_expand(cldm_testrec_prefix)) - 1);
                return -1;
            }

            cldm_rbtree_insert(tree, &record->rbnode, cldm_testrec_compare);
        }
    }

    return (ssize_t)cldm_rbtree_size(tree);
}

ssize_t cldm_collect_from(struct cldm_rbtree *restrict tree, struct cldm_elfmap const *restrict map, char *const *restrict files, size_t nfiles) {
    struct cldm_ht_entry *entries;
    struct cldm_testrec *record;
    struct cldm_ht ht;
    bool success;

    success = false;
    ht = cldm_ht_init();

    entries = malloc(nfiles * sizeof(*entries));
    if(!entries) {
        cldm_err("Could not allocate nodes for hashing");
        goto epilogue;
    }

    for(unsigned i = 0; i < nfiles; i++) {
        entries[i] = cldm_ht_mkentry_str(files[i]);
        if(!cldm_ht_insert(&ht, &entries[i])) {
            cldm_err("Could not insert %s in hash table", files[i]);
            goto epilogue;
        }
    }

    for(size_t i = 0; i < map->strtab.size; i += strlen(map->strtab.addr + i) + 1) {
        if(strncmp(cldm_str_expand(cldm_testrec_prefix), map->strtab.addr + i, sizeof(cldm_str_expand(cldm_testrec_prefix)) - 1) == 0) {
            record = cldm_elf_testrec(map, map->strtab.addr + i);
            if(!record) {
                cldm_err("Could not map test record for %s", map->strtab.addr + i + sizeof(cldm_str_expand(cldm_testrec_prefix)) - 1);
                goto epilogue;
            }
            if(cldm_ht_find(&ht, &cldm_ht_mkentry_str(record->file))) {
                cldm_rbtree_insert(tree, &record->rbnode, cldm_testrec_compare);
            }
        }
    }
    success = true;
epilogue:
    cldm_ht_free(&ht);
    if(entries) {
        free(entries);
    }
    return success ? (ssize_t)cldm_rbtree_size(tree) : -1;
}
