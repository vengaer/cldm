#ifndef CLDM_RBTREE_H
#define CLDM_RBTREE_H

#include <stdbool.h>

enum cldm_rbcolor {
    cldm_rbcolor_black,
    cldm_rbcolor_red
};

struct cldm_rbnode {
    struct cldm_rbnode *left;
    struct cldm_rbnode *right;
    enum cldm_rbcolor color;
};

typedef struct cldm_rbnode cldm_rbtree;

#define cldm_rbtree_init()  \
    { .left = 0, .right = 0, .color = cldm_rbcolor_black }

typedef int(*cldm_rbcompare)(struct cldm_rbnode const *restrict, struct cldm_rbnode const *restrict);

bool cldm_rbtree_insert(cldm_rbtree *restrict tree, struct cldm_rbnode *restrict node, cldm_rbcompare compare);
struct cldm_rbnode *cldm_rbtree_find(cldm_rbtree *restrict tree, struct cldm_rbnode const *restrict node, cldm_rbcompare compare);
struct cldm_rbnode *cldm_rbtree_remove(cldm_rbtree *restrict tree, struct cldm_rbnode const *restrict node, cldm_rbcompare compare);

#endif /* CLDM_RBTREE_H */
