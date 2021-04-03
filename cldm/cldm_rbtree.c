#include "cldm_rbtree.h"

#include <stdbool.h>

enum { CLDM_MAX_RBTREE_DEPTH = 64 };

#define link(node, idx)     \
    (*(struct cldm_rbnode **)((unsigned char *)&(node)->left + ((unsigned char *)&(node)->right - (unsigned char *)&(node)->left) * (idx)))

struct cldm_rbstack_node {
    struct cldm_rbnode *node;
    enum cldm_rbdir dir;
};

#define cldm_rbstack_init() \
    { .top = 0 }

struct cldm_rbstack {
    struct cldm_rbstack_node data[CLDM_MAX_RBTREE_DEPTH];
    unsigned top;
};

void cldm_rbtree_clear(struct cldm_rbtree *tree);
size_t cldm_rbtree_size(struct cldm_rbtree const *tree);
bool cldm_rbnode_has_child(struct cldm_rbnode const *node, enum cldm_rbdir dir);
struct cldm_rbnode *cldm_rbnode_successor(struct cldm_rbnode *node);
bool cldm_rbtree_empty(struct cldm_rbtree const *tree);

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

static inline void cldm_rbnode_make_leaf(struct cldm_rbnode *node) {
    node->flags = CLDM_RBLEAF;
}

static inline bool cldm_rbnode_is_thread(struct cldm_rbnode const *node, enum cldm_rbdir dir) {
    return node->flags & (1 << dir);
}

static inline void cldm_rbnode_unset_thread(struct cldm_rbnode *node, enum cldm_rbdir dir) {
    node->flags &= ~(1 << dir);
}

static inline void cldm_rbnode_set_thread(struct cldm_rbnode *node, enum cldm_rbdir dir) {
    node->flags |= (1 << dir);
}

static inline void cldm_rbnode_assign_thread_from(struct cldm_rbnode *restrict to, struct cldm_rbnode const *restrict from, enum cldm_rbdir dir) {
    to->flags = (to->flags & ~(1 << dir)) | (from->flags & (1 << dir));
}

static inline bool cldm_rbnode_is_red_safe(struct cldm_rbnode const *node, enum cldm_rbdir dir) {
    return !cldm_rbnode_is_thread(node, dir) && cldm_rbnode_is_red(link(node, dir));
}

static struct cldm_rbnode *cldm_rbtree_rotate_single(struct cldm_rbnode *root, enum cldm_rbdir dir) {
    struct cldm_rbnode *n = link(root, !dir);

    if(cldm_rbnode_is_thread(n, dir)) {
        cldm_rbnode_set_thread(root, !dir);
        cldm_rbnode_unset_thread(n, dir);
    }
    else {
        link(root, !dir) = link(n, dir);
    }
    link(n, dir) = root;

    cldm_rbnode_make_red(root);
    cldm_rbnode_make_black(n);

    return n;
}

static inline struct cldm_rbnode *cldm_rbtree_rotate_double(struct cldm_rbnode *root, enum cldm_rbdir dir) {
    link(root, !dir) = cldm_rbtree_rotate_single(link(root, !dir), !dir);
    return cldm_rbtree_rotate_single(root, dir);
}

static struct cldm_rbnode *cldm_rbtree_balance_insertion(struct cldm_rbnode *node, enum cldm_rbdir dir) {
    if(cldm_rbnode_is_red_safe(node, dir)) {
        if(cldm_rbnode_is_red_safe(node, !dir)) {
            cldm_rbnode_make_red(node);
            cldm_rbnode_make_black(node->left);
            cldm_rbnode_make_black(node->right);
        }
        else if(cldm_rbnode_is_red_safe(link(node, dir), dir)) {
            node = cldm_rbtree_rotate_single(node, !dir);
        }
        else if(cldm_rbnode_is_red_safe(link(node, dir), !dir)) {
            node = cldm_rbtree_rotate_double(node, !dir);
        }
    }
    return node;
}

static void cldm_rbtree_balance_removal(struct cldm_rbnode *restrict n, struct cldm_rbnode **restrict parent, struct cldm_rbnode **restrict gparent, enum cldm_rbdir dir) {
    enum cldm_rbdir pdir = (enum cldm_rbdir) ((*parent)->right == n);
    enum cldm_rbdir gpdir = (enum cldm_rbdir) ((*gparent)->right == *parent);

    struct cldm_rbnode *sibling = link(*parent, !pdir);

    if(cldm_rbnode_is_red_safe(n, !dir)) {
        link(*parent, pdir) = cldm_rbtree_rotate_single(n, dir);
        *gparent = *parent;
        *parent = link(*parent, pdir);
    }
    else if(sibling) {
        if(cldm_rbnode_is_red_safe(sibling, cldm_rbdir_left) || cldm_rbnode_is_red_safe(sibling, cldm_rbdir_right)) {
            if(cldm_rbnode_is_red_safe(sibling, pdir)) {
                link(*gparent, gpdir) = cldm_rbtree_rotate_double(*parent, pdir);
            }
            else if(cldm_rbnode_is_red_safe(sibling, !pdir)) {
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

static struct cldm_rbnode *cldm_rbtree_find_parent_of(struct cldm_rbtree *restrict tree, struct cldm_rbnode const *restrict node, cldm_rbcompare compare) {
    int relation;

    struct cldm_rbnode *parent = &tree->sentinel;
    struct cldm_rbnode *n = cldm_rbroot(tree);
    enum cldm_rbdir dir = cldm_rbdir_left;

    while(!cldm_rbnode_is_thread(parent, dir) && n != node) {
        relation = compare(n, node);

        dir = (enum cldm_rbdir) (relation > 0);
        parent = n;
        n = link(n, dir);
    }

    return parent;
}

bool cldm_rbtree_insert(struct cldm_rbtree *restrict tree, struct cldm_rbnode *restrict node, cldm_rbcompare compare) {
    struct cldm_rbnode *child = 0;
    struct cldm_rbstack stack = cldm_rbstack_init();
    struct cldm_rbnode *n;
    enum cldm_rbdir dir;
    int relation;

    if(cldm_rbnode_is_thread(&tree->sentinel, cldm_rbdir_left)) {
        *node = (struct cldm_rbnode) {
            .left = &tree->sentinel,
            .right = &tree->sentinel,
            .color = cldm_rbcolor_black,
            .flags = CLDM_RBLEAF
        };
        cldm_rbroot(tree) = node;
        cldm_rbnode_unset_thread(&tree->sentinel, cldm_rbdir_left);
        tree->size = 1u;
        cldm_rbnode_make_black(node);
        return true;
    }

    n = cldm_rbroot(tree);

    while(1) {
        relation = compare(n, node);

        if(!relation) {
            /* Already in tree */
            return false;
        }

        dir = (enum cldm_rbdir) (relation > 0);
        if(cldm_rbnode_is_thread(n, dir)) {
            break;
        }

        cldm_rbstack_push(&stack, &(struct cldm_rbstack_node){ .node = n, .dir = dir });
        n = link(n, dir);
    }

    cldm_rbnode_make_red(node);
    cldm_rbnode_make_leaf(node);
    link(node, dir) = link(n, dir);
    link(node, !dir) = n;

    link(n, dir) = node;
    cldm_rbnode_unset_thread(n, dir);

    while(!cldm_rbstack_empty(&stack)) {
        child = cldm_rbtree_balance_insertion(n, dir);

        dir = cldm_rbstack_peek(&stack)->dir;
        n = cldm_rbstack_pop(&stack)->node;

        link(n, dir) = child;
    }

    cldm_rbroot(tree) = cldm_rbtree_balance_insertion(n, dir);
    cldm_rbnode_make_black(cldm_rbroot(tree));
    ++tree->size;

    return true;
}

struct cldm_rbnode *cldm_rbtree_find(struct cldm_rbtree *restrict tree, struct cldm_rbnode const *restrict node, cldm_rbcompare compare) {
    int relation;
    struct cldm_rbnode *parent = &tree->sentinel;
    struct cldm_rbnode *n = cldm_rbroot(tree);
    enum cldm_rbdir dir = cldm_rbdir_left;

    while(!cldm_rbnode_is_thread(parent, dir)) {
        relation = compare(n, node);

        if(!relation) {
            return n;
        }

        dir = (enum cldm_rbdir) (relation > 0);
        parent = n;
        n = link(n, dir);
    }

    return 0;
}

struct cldm_rbnode *cldm_rbtree_remove(struct cldm_rbtree *restrict tree, struct cldm_rbnode const *restrict node, cldm_rbcompare compare) {
    if(!cldm_rbroot(tree)) {
        return 0;
    }

    struct cldm_rbnode *gparent = &(struct cldm_rbnode){ .left = &tree->sentinel };
    struct cldm_rbnode *parent = &tree->sentinel;
    struct cldm_rbnode *n = cldm_rbroot(tree);

    struct cldm_rbnode *found = 0;
    struct cldm_rbnode *fparent;

    enum cldm_rbdir dir = cldm_rbdir_left;
    enum cldm_rbdir fdir;
    enum cldm_rbdir gpdir;

    int relation;

    while(!cldm_rbnode_is_thread(parent, dir)) {
        relation = compare(n, node);

        if(!relation) {
            found = n;
        }

        dir = (enum cldm_rbdir) (relation > 0);

        if(!cldm_rbnode_is_red(n) && !cldm_rbnode_is_red_safe(n, dir)) {
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

        /* Make gparent adopt parent's thread flag in direction gpdir */
        cldm_rbnode_assign_thread_from(gparent, parent, gpdir);

        /* Replace found with parent */
        parent->left  = (struct cldm_rbnode *)((size_t)found->left  * (found->left != parent)  + (size_t)&tree->sentinel * (found->left == parent));
        parent->right = (struct cldm_rbnode *)((size_t)found->right * (found->right != parent) + (size_t)&tree->sentinel * (found->right == parent));
        parent->color = found->color;
        parent->flags = found->flags;

        link(fparent, fdir) = parent;
        link(gparent, gpdir) = link(parent, dir);

        *found = (struct cldm_rbnode){ 0 };

        --tree->size;
    }

    if(!cldm_rbnode_is_thread(&tree->sentinel, cldm_rbdir_left)) {
        cldm_rbnode_make_black(cldm_rbroot(tree));
    }

    return found;
}

struct cldm_rbnode *cldm_rbtree_leftmost(struct cldm_rbnode *node) {
    while(!cldm_rbnode_is_thread(node, cldm_rbdir_left)) {
        node = node->left;
    }
    return node;
}
