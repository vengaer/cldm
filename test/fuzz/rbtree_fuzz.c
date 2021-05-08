#include <cldm/cldm_algo.h>
#include <cldm/cldm_rbtree.h>

#include "rbtree_fuzz.h"
#include "rbtree_utils.h"

#include <stdio.h>
#include <stdlib.h>

struct fuzznode {
    struct cldm_rbnode node;
    uint8_t value;
};

static inline int fuzznodeval(struct cldm_rbnode const *node) {
    return cldm_container(node, struct fuzznode, node, const)->value;
}

static int fuzznodecmp(struct cldm_rbnode const *restrict l, struct cldm_rbnode const *restrict r) {
    return fuzznodeval(r) - fuzznodeval(l);
}

static int qcmp(void const *l, void const *r) {
    return fuzznodeval(r) - fuzznodeval(l);
}

static void dump_tree(struct cldm_rbtree const *tree) {
    struct cldm_rbnode *iter;
    fputs("Dumping tree...\n", stderr);
    fprintf(stderr, "Size: %zu\n", cldm_rbtree_size(tree));
    cldm_rbtree_for_each(iter, tree) {
        fprintf(stderr, "Node at %p:\n", (void const *)iter);
        fprintf(stderr, "  Value: %hhu\n", (unsigned char)fuzznodeval(iter));
        fprintf(stderr, "  Color: %s\n", rbtree_strcolor(iter));
        fprintf(stderr, "  Flags: %hhu\n", iter->flags);
        fprintf(stderr, "  Left:  %s to %p\n", cldm_rbnode_has_child(iter, cldm_rbdir_left) ? "link" : "thread", (void const *)iter->left);
        fprintf(stderr, "  Right: %s to %p\n", cldm_rbnode_has_child(iter, cldm_rbdir_right) ? "link" : "thread", (void const *)iter->right);
    }
}

static inline void report_violation(int errnum, struct cldm_rbtree const *tree) {
    fprintf(stderr, "Violation encountered: %s\n", rbtree_strviolation(errnum));
    dump_tree(tree);
}

static inline void report_missing_value(struct fuzznode const *node, struct cldm_rbtree const *tree) {
    fprintf(stderr, "Value %hhu not in tree\n", (unsigned char)node->value);
    dump_tree(tree);
}

int rbtree_fuzz(uint8_t const *data, size_t size) {
    struct fuzznode *nodes;
    struct fuzznode *unique;
    struct cldm_rbtree tree;
    size_t nunique;
    int prop;
    bool crash;

    if(!size) {
        return 0;
    }

    crash = true;
    unique = 0;

    tree = cldm_rbtree_init();
    nodes = malloc(size * sizeof(*nodes));

    if(!nodes) {
        fputs("Malloc failure\n", stderr);
        goto epilogue;
    }

    unique = malloc(size * sizeof(*nodes));
    if(!unique) {
        fputs("Malloc failure\n", stderr);
        goto epilogue;
    }

    for(unsigned i = 0; i < size; i++) {
        nodes[i].value = data[i];
        cldm_rbtree_insert(&tree, &nodes[i].node, fuzznodecmp);
        prop = rbtree_adheres_to_rbproperties(cldm_rbroot(&tree), fuzznodecmp);
        if(prop < 0) {
            report_violation(prop, &tree);
            goto epilogue;
        }
    }

    memcpy(unique, nodes, size * sizeof(*nodes));
    qsort(unique, size, sizeof(*unique), qcmp);
    nunique = cldm_uniq(unique, unique, sizeof(*unique), size, qcmp);

    for(unsigned i = 0; i < nunique; i++) {
        if(!cldm_rbtree_remove(&tree, &unique[i].node, fuzznodecmp)) {
            report_missing_value(&unique[i], &tree);
            goto epilogue;
        }
        prop = rbtree_adheres_to_rbproperties(cldm_rbroot(&tree), fuzznodecmp);
        if(prop < 0) {
            report_violation(prop, &tree);
            goto epilogue;
        }
    }

    crash = false;
epilogue:
    if(nodes) {
        free(nodes);
    }
    if(unique) {
        free(unique);
    }
    if(crash) {
        abort();
    }

    return 0;
}
