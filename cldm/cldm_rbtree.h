#ifndef CLDM_RBTREE_H
#define CLDM_RBTREE_H

#include "cldm_macro.h"
#include "cldm_mem.h"
#include "cldm_rtassert.h"

#include <stdbool.h>

enum { CLDM_MAX_RBTREE_DEPTH = 1024 };

enum cldm_rbdir {
    cldm_rbdir_left,
    cldm_rbdir_right
};

enum cldm_rbcolor {
    cldm_rbcolor_black,
    cldm_rbcolor_red
};

struct cldm_rbnode {
    struct cldm_rbnode *left;
    struct cldm_rbnode *right;
    enum cldm_rbcolor color;
};

struct cldm_rbstack_node {
    struct cldm_rbnode *node;
    enum cldm_rbdir dir;
};

struct cldm_rbstack {
    struct cldm_rbstack_node data[CLDM_MAX_RBTREE_DEPTH];
    unsigned top;
};

typedef struct cldm_rbnode cldm_rbtree;

#define cldm_rbtree_init()  \
    { .left = 0, .right = 0, .color = cldm_rbcolor_black }

#define cldm_rbstack_init() \
    { .top = 0 }

typedef int(*cldm_rbcompare)(struct cldm_rbnode const *restrict, struct cldm_rbnode const *restrict);

bool cldm_rbtree_insert(cldm_rbtree *restrict tree, struct cldm_rbnode *restrict node, cldm_rbcompare compare);
struct cldm_rbnode *cldm_rbtree_find(cldm_rbtree *restrict tree, struct cldm_rbnode const *restrict node, cldm_rbcompare compare);
struct cldm_rbnode *cldm_rbtree_remove(cldm_rbtree *restrict tree, struct cldm_rbnode const *restrict node, cldm_rbcompare compare);

inline unsigned cldm_rbstack_capacity(struct cldm_rbstack const *stack) {
    return cldm_arrsize(stack->data);
}

inline unsigned cldm_rbstack_size(struct cldm_rbstack const *stack) {
    return stack->top;
}

inline void cldm_rbstack_push(struct cldm_rbstack *restrict stack, struct cldm_rbstack_node const *restrict node) {
    cldm_rtassert(cldm_rbstack_size(stack) < cldm_rbstack_capacity(stack));
    stack->data[stack->top++] = *node;
}

inline struct cldm_rbstack_node *cldm_rbstack_pop(struct cldm_rbstack *stack) {
    return &stack->data[--stack->top];
}

inline struct cldm_rbstack_node *cldm_rbstack_peek(struct cldm_rbstack *stack) {
    return &stack->data[stack->top - 1];
}

inline bool cldm_rbstack_empty(struct cldm_rbstack *stack) {
    return !cldm_rbstack_size(stack);
}

inline void cldm_rbtree_descend(struct cldm_rbnode *restrict node, struct cldm_rbstack *restrict stack) {
    while(node) {
        cldm_rbstack_push(stack, &(struct cldm_rbstack_node){ .node = node });
        node = node->left;
    }
}

#define cldm_rbtree_for_each(iter, root)    \
    for(struct cldm_rbstack cldm_cat_expand(cldm_rbtfe,__LINE__) =                  \
            ((iter) = (root)->left, (struct cldm_rbstack) cldm_rbstack_init());     \
        ((iter) || !cldm_rbstack_empty(&cldm_cat_expand(cldm_rbtfe,__LINE__))) &&   \
          (cldm_rbtree_descend(iter, &cldm_cat_expand(cldm_rbtfe,__LINE__)),        \
            (iter) = cldm_rbstack_pop(&cldm_cat_expand(cldm_rbtfe,__LINE__))->node, \
              1);                                                                   \
        (iter) = (iter)->right)


#endif /* CLDM_RBTREE_H */
