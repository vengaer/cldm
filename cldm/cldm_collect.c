#include "cldm_byteseq.h"
#include "cldm_collect.h"
#include "cldm_hash.h"
#include "cldm_log.h"
#include "cldm_macro.h"
#include "cldm_test.h"
#include "cldm_testrec.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

static void cldm_empty_setup(void) { }
static void cldm_empty_teardown(void) { }

ssize_t cldm_collect(struct cldm_rbtree *restrict tree, struct cldm_elfmap const *restrict map) {
    struct cldm_testrec *record;

    *tree = cldm_rbtree_init();

    /* Look up tests in strtab section */
    for(size_t i = 0; i < map->strtab.size; i += strlen(map->strtab.addr + i) + 1) {
        /* Identify by test prefix */
        if(strncmp(cldm_str_expand(cldm_testrec_prefix), map->strtab.addr + i, cldm_strlitlen(cldm_str_expand(cldm_testrec_prefix))) == 0) {
            record = cldm_elf_testrec(map, map->strtab.addr + i);
            if(!record) {
                cldm_err("Could not map test record for %s", map->strtab.addr + i + cldm_strlitlen(cldm_str_expand(cldm_testrec_prefix)));
                return -1;
            }

            cldm_rbtree_insert(tree, &record->rbnode, cldm_testrec_compare);
        }
    }

    return (ssize_t)cldm_rbtree_size(tree);
}

ssize_t cldm_collect_from(struct cldm_rbtree *restrict tree, struct cldm_elfmap const *restrict map, char *const *restrict idents, size_t nidents) {
    struct cldm_ht_entry *entry;
    struct cldm_testrec *record;
    char const *identifiers[2];
    struct cldm_ht ht;
    unsigned pos;
    bool success;

    *tree = cldm_rbtree_init();

    success = false;
    ht = cldm_ht_init();

    for(unsigned i = 0; i < nidents; i++) {
        if(!cldm_ht_insert(&ht, &cldm_ht_mkentry_str(idents[i]))) {
            cldm_warn("Ignoring duplicate parameter '%s'", idents[i]);
            /* exempt from validity check */
            idents[i][strlen(idents[i])] = 1;
        }
    }

    for(size_t i = 0; i < map->strtab.size; i += strlen(map->strtab.addr + i) + 1) {
        if(strncmp(cldm_str_expand(cldm_testrec_prefix), map->strtab.addr + i, sizeof(cldm_str_expand(cldm_testrec_prefix)) - 1) == 0) {
            record = cldm_elf_testrec(map, map->strtab.addr + i);
            if(!record) {
                cldm_err("Could not map test record for %s", map->strtab.addr + i + sizeof(cldm_str_expand(cldm_testrec_prefix)) - 1);
                goto epilogue;
            }
            identifiers[0] = cldm_basename(record->file);
            identifiers[1] = record->name;
            for(unsigned j = 0; j < cldm_arrsize(identifiers); j++) {
                entry = cldm_ht_find(&ht, &cldm_ht_mkentry_str(identifiers[j]));
                if(entry) {
                    /* Use null byte for storing result of validity check */
                    ((char *)entry->key)[strlen((char *)entry->key)] = 1;
                    cldm_rbtree_insert(tree, &record->rbnode, cldm_testrec_compare);
                    break;
                }
            }
        }
    }

    for(unsigned i = 0; i < nidents; i++) {
        pos = cldm_scan_lt((char const *)idents[i], 2);
        if(!idents[i][pos]) {
            cldm_err("Positional parameter '%s' does not match any file or test known to cldm", idents[i]);
            goto epilogue;
        }
    }

    success = true;
epilogue:
    cldm_ht_free(&ht);
    return success ? (ssize_t)cldm_rbtree_size(tree) : -1;
}

size_t cldm_collect_auxprocs(struct cldm_auxprocs *auxprocs, struct cldm_elfmap const *restrict map) {
    static char const *setup_idents[] = {
        cldm_str_expand(cldm_global_setup_ident),
        cldm_str_expand(cldm_local_setup_ident),
    };
    static char const *teardown_idents[] = {
        cldm_str_expand(cldm_global_teardown_ident),
        cldm_str_expand(cldm_local_teardown_ident)
    };
    cldm_setup_handle *setup_handles[] = {
        &auxprocs->global_setup,
        &auxprocs->local_setup
    };
    cldm_teardown_handle *teardown_handles[] = {
        &auxprocs->global_teardown,
        &auxprocs->local_teardown
    };

    cldm_setup_handle **setup_iter;
    cldm_teardown_handle **teardown_iter;
    char const **cciter;
    size_t nloaded;

    nloaded = 0;
    cldm_for_each_zip(cciter, setup_iter, setup_idents, setup_handles) {
        **setup_iter = cldm_elf_func(map, *cciter);
        nloaded += !!**setup_iter;
        if(!**setup_iter) {
            **setup_iter = cldm_empty_setup;
        }
    }

    cldm_for_each_zip(cciter, teardown_iter, teardown_idents, teardown_handles) {
        **teardown_iter = cldm_elf_func(map, *cciter);
        nloaded += !!**teardown_iter;
        if(!**teardown_iter) {
            **teardown_iter = cldm_empty_teardown;
        }
    }

    return nloaded;
}
