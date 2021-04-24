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
char const *rbtree_strviolation(int errnum);

inline bool rbtree_is_leaf(struct cldm_rbnode const *node) {
    return (node->flags & CLDM_RBLEAF) == CLDM_RBLEAF;
}

inline bool rbtree_is_red(struct cldm_rbnode const *node) {
    return node->color == cldm_rbcolor_red;
}

inline bool rbtree_is_black(struct cldm_rbnode const *node) {
    return node->color == cldm_rbcolor_black;
}

inline bool rbtree_is_red_safe(struct cldm_rbnode const *node, enum cldm_rbdir dir) {
    return  !(node->flags & (1 << dir)) && rbtree_is_red(dir == cldm_rbdir_left ? node->left : node->right);
}

inline char const *rbtree_strcolor(struct cldm_rbnode const *node) {
    return rbtree_is_black(node) ? "black" : "red";
}


#endif /* RBTREE_UTILS_H */
