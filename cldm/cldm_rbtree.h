#ifndef CLDM_RBTREE_H
#define CLDM_RBTREE_H

#include "cldm_macro.h"

#include "cldm_rtassert.h"

#include <stdbool.h>

enum cldm_rbdir {
    cldm_rbdir_left,
    cldm_rbdir_right
};

enum cldm_rbcolor {
    cldm_rbcolor_black,
    cldm_rbcolor_red
};

enum {
    CLDM_RBTHREADL = 0x01,
    CLDM_RBTHREADR = 0x02,
    CLDM_RBLEAF = CLDM_RBTHREADL | CLDM_RBTHREADR
};

typedef unsigned char cldm_rbnode_flags;

struct cldm_rbnode {
    struct cldm_rbnode *left;
    struct cldm_rbnode *right;
    enum cldm_rbcolor color;
    cldm_rbnode_flags flags;
};

struct cldm_rbtree {
    struct cldm_rbnode sentinel;
    size_t size;
};

#define cldm_rbtree_init()              \
    { .sentinel = { .flags = CLDM_RBLEAF }, .size = 0u }

#define cldm_rbroot(tree)   \
    (tree)->sentinel.left

typedef int(*cldm_rbcompare)(struct cldm_rbnode const *restrict, struct cldm_rbnode const *restrict);

bool cldm_rbtree_insert(struct cldm_rbtree *restrict tree, struct cldm_rbnode *restrict node, cldm_rbcompare compare);
struct cldm_rbnode *cldm_rbtree_find(struct cldm_rbtree *restrict tree, struct cldm_rbnode const *restrict node, cldm_rbcompare compare);
struct cldm_rbnode *cldm_rbtree_remove(struct cldm_rbtree *restrict tree, struct cldm_rbnode const *restrict node, cldm_rbcompare compare);
struct cldm_rbnode *cldm_rbtree_leftmost(struct cldm_rbnode *node);

inline void cldm_rbtree_clear(struct cldm_rbtree *tree) {
    tree->sentinel.flags = CLDM_RBLEAF;
    tree->size = 0u;
}

inline bool cldm_rbtree_empty(struct cldm_rbtree const *tree) {
    return tree->sentinel.flags == CLDM_RBLEAF;
}

inline size_t cldm_rbtree_size(struct cldm_rbtree const *tree) {
    return tree->size;
}

inline bool cldm_rbnode_has_child(struct cldm_rbnode const *node, enum cldm_rbdir dir) {
    return !(node->flags & (1 << dir));
}

inline struct cldm_rbnode *cldm_rbnode_successor(struct cldm_rbnode *node) {
    return cldm_rbnode_has_child(node, cldm_rbdir_right) ? cldm_rbtree_leftmost(node->right) : node->right;
}

#define cldm_rbtree_for_each(iter, tree)                        \
    for(iter = cldm_rbtree_leftmost(cldm_rbroot(tree));         \
        iter != &(tree)->sentinel;                              \
        iter = cldm_rbnode_successor(iter))


#endif /* CLDM_RBTREE_H */
