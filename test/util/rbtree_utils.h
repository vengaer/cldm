#ifndef RBTREE_UTILS_H
#define RBTREE_UTILS_H

#include <cldm/cldm_rbtree.h>

#include <stdbool.h>

enum {
    RBTREE_RED_VIOLATION         = -0x1,
    RBTREE_LEFT_CHILD_VIOLATION  = -0x2,
    RBTREE_RIGHT_CHILD_VIOLATION = -0x4,
    RBTREE_HEIGHT_VIOLATION      = -0x8
};

int rbtree_adheres_to_rbproperties(struct cldm_rbnode const *node, int(*compare)(struct cldm_rbnode const *restrict, struct cldm_rbnode const *restrict));

inline bool is_leaf(struct cldm_rbnode const *node) {
    return (node->flags & CLDM_RBLEAF) == CLDM_RBLEAF;
}

inline bool is_red(struct cldm_rbnode const *node) {
    return node->color == cldm_rbcolor_red;
}

inline bool is_black(struct cldm_rbnode const *node) {
    return node->color == cldm_rbcolor_black;
}

inline bool is_red_safe(struct cldm_rbnode const *node, enum cldm_rbdir dir) {
    return  !(node->flags & (1 << dir)) && is_red(dir == cldm_rbdir_left ? node->left : node->right);
}


#endif /* RBTREE_UTILS_H */
