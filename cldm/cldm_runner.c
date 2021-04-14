#include "cldm_hash.h"
#include "cldm_log.h"
#include "cldm_macro.h"
#include "cldm_ntbs.h"
#include "cldm_rbtree.h"
#include "cldm_runner.h"
#include "cldm_test.h"

#include <stdlib.h>
#include <string.h>

#include <sys/types.h>

static bool cldm_insert_file(struct cldm_ht *restrict ht, struct cldm_rbnode *restrict begin, struct cldm_rbnode *restrict end, char const *restrict file, struct cldm_rbht_node *restrict node, size_t ntests) {
    *node = (struct cldm_rbht_node) {
        .begin = begin,
        .end = end,
        .ntests = ntests,
        .entry = cldm_ht_mkentry_str(file)
    };

    if(!cldm_ht_insert(ht, &node->entry)) {
        cldm_err("Hash table insertion failed");
        return false;
    }

    return true;
}

static ssize_t cldm_collect_files(struct cldm_ht *restrict ht, struct cldm_rbtree const *restrict tests, struct cldm_rbht_node *restrict nodes) {
    struct cldm_testrec const *record;
    struct cldm_rbnode *begin;
    struct cldm_rbnode *iter;
    char const *basename;
    char const *file;
    unsigned nodeidx;
    size_t ntests;

    file = 0;
    nodeidx = 0;
    ntests = 0;

    begin = cldm_rbtree_leftmost(cldm_rbroot(tests));

    cldm_rbtree_for_each(iter, tests) {
        record = cldm_testrec_get(iter, const);
        basename = cldm_basename(record->file);
        /* Insert 1 record per file */
        if(file && strcmp(file, basename)) {
            if(!cldm_insert_file(ht, begin, iter, file, &nodes[nodeidx++], ntests)) {
                return -1;
            }
            begin = iter;
            ntests = 0;
        }
        file = basename;
        ++ntests;
    }

    if(file) {
        if(!cldm_insert_file(ht, begin, iter, file, &nodes[nodeidx++], ntests)) {
            return -1;
        }
    }

    return cldm_ht_size(ht);
}

static int cldm_collect_and_run_specified(struct cldm_rbtree const *restrict tests, struct cldm_elfmap const *restrict map, bool fail_fast, char **restrict files, size_t nfiles) {
    struct cldm_rbht_node *nodes;
    struct cldm_rbht_node *n;
    struct cldm_ht_entry *entry;
    struct cldm_ht ht;
    char **iter;
    int status;
    size_t ntests;

    status = -1;
    ntests = 0;
    nodes = malloc(cldm_rbtree_size(tests) * sizeof(*nodes));

    if(!nodes) {
        cldm_err("Could not allocate memory for hashing");
        return -1;
    }

    ht = cldm_ht_init();
    if(cldm_collect_files(&ht, tests, nodes) < 0) {
        goto epilogue;
    }

    /* Verify that specified files are compiled into binary and compute number of tests */
    cldm_for_each(iter, files, nfiles) {
        entry = cldm_ht_find(&ht, &cldm_ht_mkentry_str(*iter));
        if(!entry) {
            cldm_err("File '%s' not found", *iter);
            goto epilogue;
        }
        n = cldm_container(entry, struct cldm_rbht_node, entry);
        ntests += n->ntests;
    }

    status = cldm_test_invoke_specified(&ht, map, fail_fast, ntests, files, nfiles);
epilogue:
    free(nodes);
    cldm_ht_free(&ht);
    return status;
}

int cldm_collect_and_run(struct cldm_elfmap const *restrict map, bool fail_fast, char **restrict files, size_t nfiles) {
    struct cldm_rbtree tests;

    tests = cldm_rbtree_init();
    if(cldm_test_collect(&tests, map) < 0) {
        cldm_err("Error collecting tests");
        return -1;
    }

    if(!nfiles) {
        /* No files specified on command line, run all tests */
        return cldm_test_invoke_each(&tests, map, fail_fast);
    }

    return cldm_collect_and_run_specified(&tests, map, fail_fast, files, nfiles);
}
