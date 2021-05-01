#include "cldm_rbtree.h"

#include <limits.h>
#include <stdbool.h>

enum { CLDM_MAX_RBTREE_DEPTH = sizeof(size_t) * CHAR_BIT };

#define link(node, idx)     \
    (*(struct cldm_rbnode **)((unsigned char *)&(node)->left + ((unsigned char *)&(node)->right - (unsigned char *)&(node)->left) * (idx)))

void cldm_rbtree_clear(struct cldm_rbtree *tree);
bool cldm_rbtree_empty(struct cldm_rbtree const *tree);
size_t cldm_rbtree_size(struct cldm_rbtree const *tree);
bool cldm_rbnode_has_child(struct cldm_rbnode const *node, enum cldm_rbdir dir);
struct cldm_rbnode *cldm_rbnode_successor(struct cldm_rbnode *node);

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
        /* Links already referring to correct nodes,
         * enough to modify thread flags */
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

static void cldm_rbtree_balance_insertion(struct cldm_rbnode *restrict n, struct cldm_rbnode *restrict p, struct cldm_rbnode *restrict gp, struct cldm_rbnode *restrict ggp) {
    enum cldm_rbdir pdir;
    enum cldm_rbdir gpdir;
    enum cldm_rbdir ggpdir;

    cldm_rbnode_make_red(n);

    if(!cldm_rbnode_is_thread(n, cldm_rbdir_left) && !cldm_rbnode_is_thread(n, cldm_rbdir_right)) {
        cldm_rbnode_make_black(n->left);
        cldm_rbnode_make_black(n->right);
    }

    if(cldm_rbnode_is_red(p)) {
        cldm_rbnode_make_red(gp);

        pdir = (enum cldm_rbdir)(p->right == n);
        gpdir = (enum cldm_rbdir)(gp->right == p);
        ggpdir = (enum cldm_rbdir)(ggp->right == gp);

        if(pdir != gpdir) {
            link(ggp, ggpdir) = cldm_rbtree_rotate_double(gp, !gpdir);
            cldm_rbnode_make_black(n);
        }
        else {
            link(ggp, ggpdir) = cldm_rbtree_rotate_single(gp, !gpdir);
            cldm_rbnode_make_black(p);
        }
    }
}

static void cldm_rbtree_balance_removal(struct cldm_rbnode *restrict n, struct cldm_rbnode **restrict p, struct cldm_rbnode **restrict gp, enum cldm_rbdir dir) {
    enum cldm_rbdir pdir;
    enum cldm_rbdir gpdir;

    struct cldm_rbnode *sibling;

    pdir = (enum cldm_rbdir) ((*p)->right == n);
    gpdir = (enum cldm_rbdir) ((*gp)->right == *p);

    if(cldm_rbnode_is_red_safe(n, !dir)) {
        link(*p, pdir) = cldm_rbtree_rotate_single(n, dir);
        *p = link(*p, pdir);
    }
    else if(!cldm_rbnode_is_thread(*p, !pdir)) {
        sibling = link(*p, !pdir);
        if(cldm_rbnode_is_red_safe(sibling, cldm_rbdir_left) || cldm_rbnode_is_red_safe(sibling, cldm_rbdir_right)) {
            if(cldm_rbnode_is_red_safe(sibling, pdir)) {
                link(*gp, gpdir) = cldm_rbtree_rotate_double(*p, pdir);
            }
            else if(cldm_rbnode_is_red_safe(sibling, !pdir)) {
                link(*gp, gpdir) = cldm_rbtree_rotate_single(*p, pdir);
            }

            cldm_rbnode_make_red(n);
            cldm_rbnode_make_red(link(*gp, gpdir));
            cldm_rbnode_make_black(link(*gp, gpdir)->left);
            cldm_rbnode_make_black(link(*gp, gpdir)->right);
        }
        else {
            cldm_rbnode_make_red(n);
            cldm_rbnode_make_red(sibling);
            cldm_rbnode_make_black(*p);
        }
    }
}

static struct cldm_rbnode *cldm_rbtree_find_parent_of(struct cldm_rbtree *restrict tree, struct cldm_rbnode const *restrict node, cldm_rbcompare compare) {
    int relation;

    struct cldm_rbnode *p = &tree->sentinel;
    struct cldm_rbnode *n = cldm_rbroot(tree);
    enum cldm_rbdir dir = cldm_rbdir_left;

    while(!cldm_rbnode_is_thread(p, dir) && n != node) {
        relation = compare(n, node);

        dir = (enum cldm_rbdir) (relation > 0);
        p = n;
        n = link(n, dir);
    }

    return p;
}

bool cldm_rbtree_insert(struct cldm_rbtree *restrict tree, struct cldm_rbnode *restrict node, cldm_rbcompare compare) {
    struct cldm_rbnode *ggp;
    struct cldm_rbnode *gp;
    struct cldm_rbnode *p;
    struct cldm_rbnode *n;

    enum cldm_rbdir dir;
    int relation;

    if(cldm_rbtree_empty(tree)) {
        *node = (struct cldm_rbnode) {
            .left = &tree->sentinel,
            .right = &tree->sentinel,
            .color = cldm_rbcolor_black,
            .flags = CLDM_RBLEAF
        };
        cldm_rbroot(tree) = node;
        cldm_rbnode_unset_thread(&tree->sentinel, cldm_rbdir_left);
        tree->sentinel.right = &tree->sentinel;
        tree->size = 1u;
        cldm_rbnode_make_black(node);
        return true;
    }

    ggp = &tree->sentinel;
    gp = &tree->sentinel;
    p = &tree->sentinel;
    n = cldm_rbroot(tree);

    while(1) {
        if(cldm_rbnode_is_red_safe(n, cldm_rbdir_left) && cldm_rbnode_is_red_safe(n, cldm_rbdir_right)) {
            /* Percolate red coloring upwards */
            cldm_rbtree_balance_insertion(n, p, gp, ggp);
        }
        relation = compare(n, node);

        if(!relation) {
            /* Already in tree */
            cldm_rbnode_make_black(cldm_rbroot(tree));
            return false;
        }

        dir = (enum cldm_rbdir) (relation > 0);
        if(cldm_rbnode_is_thread(n, dir)) {
            break;
        }

        ggp = gp;
        gp = p;
        p = n;
        n = link(n, dir);
    }

    /* Prepare node */
    cldm_rbnode_make_leaf(node);
    link(node, dir) = link(n, dir);
    link(node, !dir) = n;

    /* Set child of n */
    link(n, dir) = node;
    cldm_rbnode_unset_thread(n, dir);

    /* Ensure properties are upheld */
    cldm_rbtree_balance_insertion(node, n, p, gp);
    cldm_rbnode_make_black(cldm_rbroot(tree));

    ++tree->size;

    return true;
}

struct cldm_rbnode *cldm_rbtree_find(struct cldm_rbtree *restrict tree, struct cldm_rbnode const *restrict node, cldm_rbcompare compare) {
    int relation;
    struct cldm_rbnode *p = &tree->sentinel;
    struct cldm_rbnode *n = cldm_rbroot(tree);
    enum cldm_rbdir dir = cldm_rbdir_left;

    while(!cldm_rbnode_is_thread(p, dir)) {
        relation = compare(n, node);

        if(!relation) {
            return n;
        }

        dir = (enum cldm_rbdir) (relation > 0);
        p = n;
        n = link(n, dir);
    }

    return 0;
}

struct cldm_rbnode *cldm_rbtree_remove(struct cldm_rbtree *restrict tree, struct cldm_rbnode const *restrict node, cldm_rbcompare compare) {
    if(!cldm_rbroot(tree)) {
        return 0;
    }

    struct cldm_rbnode *gp = &(struct cldm_rbnode){ .left = &tree->sentinel };
    struct cldm_rbnode *p = &tree->sentinel;
    struct cldm_rbnode *n = cldm_rbroot(tree);

    struct cldm_rbnode *found = 0;
    struct cldm_rbnode *fparent;

    enum cldm_rbdir dir = cldm_rbdir_left;
    enum cldm_rbdir fdir;
    enum cldm_rbdir gpdir;

    int relation;

    while(!cldm_rbnode_is_thread(p, dir)) {
        relation = compare(n, node);

        if(!relation) {
            found = n;
        }

        dir = (enum cldm_rbdir) (relation > 0);

        /* Balance while descending */
        if(!cldm_rbnode_is_red(n) && !cldm_rbnode_is_red_safe(n, dir)) {
            cldm_rbtree_balance_removal(n, &p, &gp, dir);
        }

        gp = p;
        p = n;
        n = link(n, dir);
    }

    if(found) {
        fparent = cldm_rbtree_find_parent_of(tree, found, compare);

        fdir = (enum cldm_rbdir) (fparent->right == found);
        gpdir = (enum cldm_rbdir) (gp->right == p);

        /* Make gp adopt p's thread flag in direction gpdir */
        cldm_rbnode_assign_thread_from(gp, p, gpdir);

        /* Replace found with p */
        p->left  = (struct cldm_rbnode *)((size_t)found->left  * (found->left != p)  + (size_t)&tree->sentinel * (found->left == p));
        p->right = (struct cldm_rbnode *)((size_t)found->right * (found->right != p) + (size_t)&tree->sentinel * (found->right == p));
        p->color = found->color;
        p->flags = found->flags;

        link(fparent, fdir) = p;
        link(gp, gpdir) = link(p, dir);

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
