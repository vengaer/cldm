#include "cldm_macro.h"
#include "cldm_mem.h"
#include "cldm_rbtree.h"
#include "cldm_rtassert.h"

#include <stdbool.h>

enum { CLDM_MAX_RBTREE_DEPTH = 1024 };

enum cldm_rbdir {
    cldm_rbdir_left,
    cldm_rbdir_right
};

struct cldm_rbstack_node {
    struct cldm_rbnode *node;
    enum cldm_rbdir dir;
};

struct cldm_rbstack {
    struct cldm_rbstack_node data[CLDM_MAX_RBTREE_DEPTH];
    unsigned top;
};

#define link(node, idx)     \
    (*(struct cldm_rbnode **)((unsigned char *)&(node)->left + ((unsigned char *)&(node)->right - (unsigned char*)&(node)->left) * (idx)))

#define cldm_rbstack_init() \
    { .top = 0 }

static inline unsigned cldm_rbstack_capacity(struct cldm_rbstack const *stack) {
    return cldm_arrsize(stack->data);
}

static inline unsigned cldm_rbstack_size(struct cldm_rbstack const *stack) {
    return stack->top;
}

static inline void cldm_rbstack_push(struct cldm_rbstack *restrict stack, struct cldm_rbstack_node const *restrict node) {
    cldm_rtassert(cldm_rbstack_size(stack) < cldm_rbstack_capacity(stack));
    stack->data[stack->top++] = *node;
}

static inline struct cldm_rbstack_node *cldm_rbstack_pop(struct cldm_rbstack *stack) {
    return &stack->data[--stack->top];
}

static inline struct cldm_rbstack_node *cldm_rbstack_peek(struct cldm_rbstack *stack) {
    return &stack->data[stack->top - 1];
}

static inline bool cldm_rbstack_empty(struct cldm_rbstack *stack) {
    return !cldm_rbstack_size(stack);
}

static inline void cldm_rbnode_make_red(struct cldm_rbnode *node) {
    node->color = cldm_rbcolor_red;
}

static inline void cldm_rbnode_make_black(struct cldm_rbnode *node) {
    node->color = cldm_rbcolor_black;
}

static inline bool cldm_rbnode_is_red(struct cldm_rbnode const *node) {
    return node->color == cldm_rbcolor_red;
}

static inline bool cldm_rbnode_is_red_safe(struct cldm_rbnode const *node) {
    return node && cldm_rbnode_is_red(node);
}

static struct cldm_rbnode *cldm_rbtree_rotate_single(struct cldm_rbnode *root, enum cldm_rbdir dir) {
    struct cldm_rbnode *n = link(root, !dir);

    link(root, !dir) = link(n, dir);
    link(n, dir) = root;

    cldm_rbnode_make_red(root);
    cldm_rbnode_make_black(n);

    return n;
}

static struct cldm_rbnode *cldm_rbtree_rotate_double(struct cldm_rbnode *root, enum cldm_rbdir dir) {
    link(root, !dir) = cldm_rbtree_rotate_single(link(root, !dir), !dir);
    return cldm_rbtree_rotate_single(root, dir);
}

static struct cldm_rbnode *cldm_rbtree_balance_insertion(struct cldm_rbnode *node, enum cldm_rbdir dir) {
    if(cldm_rbnode_is_red_safe(link(node, dir))) {
        if(cldm_rbnode_is_red_safe(link(node, !dir))) {
            cldm_rbnode_make_red(node);
            cldm_rbnode_make_black(node->left);
            cldm_rbnode_make_black(node->right);
        }
        else if(cldm_rbnode_is_red_safe(link(link(node, dir), dir))) {
            node = cldm_rbtree_rotate_single(node, !dir);
        }
        else if(cldm_rbnode_is_red_safe(link(link(node, dir), !dir))) {
            node = cldm_rbtree_rotate_double(node, !dir);
        }
    }
    return node;
}

static void cldm_rbtree_balance_removal(struct cldm_rbnode *restrict n, struct cldm_rbnode **restrict parent, struct cldm_rbnode **restrict gparent, enum cldm_rbdir dir) {
    enum cldm_rbdir pdir = (enum cldm_rbdir) ((*parent)->right == n);
    enum cldm_rbdir gpdir = (enum cldm_rbdir) ((*gparent)->right == *parent);

    struct cldm_rbnode *sibling = link(*parent, !pdir);

    if(cldm_rbnode_is_red_safe(link(n, !dir))) {
        link(*parent, pdir) = cldm_rbtree_rotate_single(n, dir);
        *gparent = *parent;
        *parent = link(*parent, pdir);
    }
    else if(sibling) {
        if(cldm_rbnode_is_red_safe(sibling->left) || cldm_rbnode_is_red_safe(sibling->right)) {
            if(cldm_rbnode_is_red_safe(link(sibling, pdir))) {
                link(*gparent, gpdir) = cldm_rbtree_rotate_double(*parent, pdir);
            }
            else if(cldm_rbnode_is_red_safe(link(sibling, !pdir))) {
                link(*gparent, gpdir) = cldm_rbtree_rotate_single(*parent, pdir);
            }

            cldm_rbnode_make_red(n);
            cldm_rbnode_make_red(link(*gparent, gpdir));
            cldm_rbnode_make_black(link(*gparent, gpdir)->left);
            cldm_rbnode_make_black(link(*gparent, gpdir)->right);

            *gparent = link(*gparent, gpdir);
        }
        else {
            cldm_rbnode_make_red(n);
            cldm_rbnode_make_red(sibling);
            cldm_rbnode_make_black(*parent);
        }
    }
}

static struct cldm_rbnode *cldm_rbtree_find_parent_of(cldm_rbtree *restrict tree, struct cldm_rbnode const *restrict node, cldm_rbcompare compare) {
    int relation;

    struct cldm_rbnode *parent = tree;
    struct cldm_rbnode *n = tree->left;

    enum cldm_rbdir dir;

    while(parent && n != node) {
        relation = compare(n, node);

        dir = (enum cldm_rbdir) (relation > 0);
        parent = n;
        n = link(n, dir);
    }

    return parent;
}

bool cldm_rbtree_insert(cldm_rbtree *restrict tree, struct cldm_rbnode *restrict node, cldm_rbcompare compare) {
    struct cldm_rbnode *child = 0;
    struct cldm_rbstack stack = cldm_rbstack_init();
    struct cldm_rbnode *n;
    enum cldm_rbdir dir;
    int relation;

    cldm_mset(node, 0, sizeof(*node));

    if(!tree->left) {
        tree->left = node;
        cldm_rbnode_make_black(node);
        return true;
    }

    n = tree->left;

    while(n) {
        relation = compare(n, node);

        if(!relation) {
            /* Already in tree */
            return false;
        }

        dir = (enum cldm_rbdir) (relation > 0);

        cldm_rbstack_push(&stack, &(struct cldm_rbstack_node){ .node = n, .dir = dir });
        n = link(n, dir);
    }

    cldm_rbnode_make_red(node);
    n = cldm_rbstack_pop(&stack)->node;
    link(n, relation > 0) = node;

    while(!cldm_rbstack_empty(&stack)) {
        child = cldm_rbtree_balance_insertion(n, dir);

        dir = cldm_rbstack_peek(&stack)->dir;
        n = cldm_rbstack_pop(&stack)->node;

        link(n, dir) = child;
    }

    tree->left = cldm_rbtree_balance_insertion(n, dir);
    cldm_rbnode_make_black(tree->left);

    return true;
}

struct cldm_rbnode *cldm_rbtree_find(cldm_rbtree *restrict tree, struct cldm_rbnode const *restrict node, cldm_rbcompare compare) {
    int relation;
    struct cldm_rbnode *n = tree->left;

    while(n) {
        relation = compare(n, node);

        if(!relation) {
            return n;
        }
        n = link(n, relation > 0);
    }

    return n;
}

struct cldm_rbnode *cldm_rbtree_remove(cldm_rbtree *restrict tree, struct cldm_rbnode const *restrict node, cldm_rbcompare compare) {
    if(!tree->left) {
        return 0;
    }

    struct cldm_rbnode faux_root = { .left = tree };

    struct cldm_rbnode *gparent = &faux_root;
    struct cldm_rbnode *parent = tree;
    struct cldm_rbnode *n = tree->left;

    struct cldm_rbnode *found = 0;
    struct cldm_rbnode *fparent;

    enum cldm_rbdir dir = cldm_rbdir_left;
    enum cldm_rbdir fdir;
    enum cldm_rbdir gpdir;

    int relation;

    while(n) {
        relation = compare(n, node);

        if(!relation) {
            found = n;
        }

        dir = (enum cldm_rbdir) (relation > 0);

        if(!cldm_rbnode_is_red(n) && !cldm_rbnode_is_red_safe(link(n, dir))) {
            cldm_rbtree_balance_removal(n, &parent, &gparent, dir);
        }

        gparent = parent;
        parent = n;
        n = link(n, dir);
    }

    if(found) {
        fparent = cldm_rbtree_find_parent_of(tree, found, compare);

        fdir = (enum cldm_rbdir) (fparent->right == found);
        gpdir = (enum cldm_rbdir) (gparent->right == parent);

        parent->left = found->left;
        parent->right = found->right;
        parent->color = found->color;

        link(fparent, fdir) = parent;
        link(gparent, gpdir) = link(parent, dir);

        found->left = 0;
        found->right = 0;
    }

    if(tree->left) {
        cldm_rbnode_make_black(tree->left);
    }

    return found;
}
