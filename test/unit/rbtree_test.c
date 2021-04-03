#include <cldm/cldm.h>
#include <cldm/cldm_macro.h>
#include <cldm/cldm_rbtree.h>

#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

enum { SIZE = 2048 };

enum {
    RED_VIOLATION         = -0x1,
    LEFT_CHILD_VIOLATION  = -0x2,
    RIGHT_CHILD_VIOLATION = -0x4,
    HEIGHT_VIOLATION      = -0x8
};

struct inode {
    struct cldm_rbnode node;
    int value;
};

static inline int rand_range(int low, int high) {
    return low + (double)(rand() / RAND_MAX) * (high - low);
}

static inline int nodeval(struct cldm_rbnode const *node) {
    return cldm_container(node, struct inode, node, const)->value;
}

static int compare(struct cldm_rbnode const *restrict l, struct cldm_rbnode const *restrict r) {
    return nodeval(r) - nodeval(l);
}

static inline bool is_leaf(struct cldm_rbnode const *node) {
    return (node->flags & CLDM_RBLEAF) == CLDM_RBLEAF;
}

static inline bool is_red(struct cldm_rbnode const *node) {
    return node->color == cldm_rbcolor_red;
}

static inline bool is_black(struct cldm_rbnode const *node) {
    return node->color == cldm_rbcolor_black;
}

static inline bool is_red_safe(struct cldm_rbnode const *node, enum cldm_rbdir dir) {
    return  !(node->flags & (1 << dir)) && is_red(dir == cldm_rbdir_left ? node->left : node->right);
}

static int adheres_to_rbproperties(struct cldm_rbnode const *node) {
    int rval;
    struct cldm_rbnode *rnode;
    int hcontrib;
    int lheight;
    int rheight;

    if(is_red(node) && (is_red_safe(node, cldm_rbdir_left) || is_red_safe(node, cldm_rbdir_right))) {
        return RED_VIOLATION;
    }

    if(cldm_rbnode_has_child(node, cldm_rbdir_left) && compare(node->left, node) <= 0) {
        return LEFT_CHILD_VIOLATION;
    }

    if(cldm_rbnode_has_child(node, cldm_rbdir_right) && compare(node, node->right) <= 0) {
        return RIGHT_CHILD_VIOLATION;
    }

    hcontrib = is_black(node);

    if(!node->flags) {
        lheight = adheres_to_rbproperties(node->left);
        if(lheight < 0) {
            return lheight;
        }
        rheight = adheres_to_rbproperties(node->right);
        if(rheight < 0) {
            return rheight;
        }

        if(lheight != rheight) {
            return HEIGHT_VIOLATION;
        }

        return lheight + hcontrib;
    }
    else if(!is_leaf(node)) {
        if(node->flags & CLDM_RBTHREADR) {
            rnode = node->left;
        }
        else {
            rnode = node->right;
        }

        rval = adheres_to_rbproperties(rnode);

        if(rval < 0) {
            return rval;
        }
        else if(rval > 0) {
            return HEIGHT_VIOLATION;
        }
    }

    return hcontrib;
}

TEST(cldm_rbtree_insert_inorder) {
    struct inode nodes[SIZE];
    struct inode *iter;

    srand(time(0));

    cldm_for_each(iter, nodes) {
        iter->value = rand_range(-100, 99);
    }

    struct cldm_rbtree tree = cldm_rbtree_init();

    cldm_for_each(iter, nodes) {
        cldm_rbtree_insert(&tree, &iter->node, compare);
        ASSERT_GE(adheres_to_rbproperties(cldm_rbroot(&tree)), 0);
    }

    cldm_rbtree_clear(&tree);
    ASSERT_TRUE(cldm_rbtree_empty(&tree));

    for(unsigned i = 0; i < cldm_arrsize(nodes); i++) {
        cldm_rbtree_insert(&tree, &nodes[cldm_arrsize(nodes) - i - 1].node, compare);
        ASSERT_GE(adheres_to_rbproperties(cldm_rbroot(&tree)), 0);
    }
}

TEST(cldm_rbtree_insert_even_odd) {
    struct inode nodes[SIZE];
    struct inode *iter;

    srand(time(0));

    cldm_for_each(iter, nodes) {
        iter->value = rand_range(-100, 99);
    }

    struct cldm_rbtree tree = cldm_rbtree_init();

    for(unsigned i = 0; i < cldm_arrsize(nodes); i += 2) {
        cldm_rbtree_insert(&tree, &nodes[i].node, compare);
        ASSERT_GE(adheres_to_rbproperties(cldm_rbroot(&tree)), 0);
    }

    for(unsigned i = 1; i < cldm_arrsize(nodes); i += 2) {
        cldm_rbtree_insert(&tree, &nodes[i].node, compare);
        ASSERT_GE(adheres_to_rbproperties(cldm_rbroot(&tree)), 0);
    }
}

TEST(cldm_rbtree_insert_reverse) {
    struct inode nodes[SIZE];
    struct inode *iter;

    srand(time(0));

    cldm_for_each(iter, nodes) {
        iter->value = rand_range(-100, 99);
    }

    struct cldm_rbtree tree = cldm_rbtree_init();

    for(unsigned i = 0; i < cldm_arrsize(nodes); i++) {
        cldm_rbtree_insert(&tree, &nodes[cldm_arrsize(nodes) - i - 1].node, compare);
        ASSERT_GE(adheres_to_rbproperties(cldm_rbroot(&tree)), 0);
    }
}

TEST(cldm_rbtree_find) {
    struct inode nodes[SIZE];

    for(unsigned i = 0; i < cldm_arrsize(nodes); i++) {
        nodes[i].value = i * 2 - cldm_arrsize(nodes) / 2;
    }

    struct cldm_rbtree tree = cldm_rbtree_init();

    for(unsigned i = 1; i < cldm_arrsize(nodes) - 1; i++) {
        cldm_rbtree_insert(&tree, &nodes[i].node, compare);
    }

    for(unsigned i = 1; i < cldm_arrsize(nodes) - 1; i++) {
        ASSERT_NE(cldm_rbtree_find(&tree, &nodes[i].node, compare), 0);
    }

    ASSERT_EQ(cldm_rbtree_find(&tree, &nodes[0].node, compare), 0);
    ASSERT_EQ(cldm_rbtree_find(&tree, &nodes[cldm_arrsize(nodes) - 1].node, compare), 0);
}

TEST(cldm_rbtree_remove_inorder) {
    struct inode nodes[SIZE];

    for(unsigned i = 0; i < cldm_arrsize(nodes); i++) {
        nodes[i].value = i - cldm_arrsize(nodes) / 2;
    }

    struct cldm_rbtree tree = cldm_rbtree_init();

    for(unsigned i = 0; i < cldm_arrsize(nodes) - 1; i++) {
        ASSERT_TRUE(cldm_rbtree_insert(&tree, &nodes[i].node, compare));
    }

    ASSERT_FALSE(cldm_rbtree_remove(&tree, &nodes[cldm_arrsize(nodes) - 1].node, compare));
    ASSERT_GE(adheres_to_rbproperties(cldm_rbroot(&tree)), 0);

    for(unsigned i = 0; i < cldm_arrsize(nodes) - 1; i++) {
        ASSERT_TRUE(cldm_rbtree_remove(&tree, &nodes[i].node, compare));
        ASSERT_GE(adheres_to_rbproperties(cldm_rbroot(&tree)), 0);
    }

    ASSERT_TRUE(cldm_rbtree_empty(&tree));
}

TEST(cldm_rbtree_remove_reverse) {
    struct inode nodes[SIZE];

    for(unsigned i = 0; i < cldm_arrsize(nodes); i++) {
        nodes[i].value = i - cldm_arrsize(nodes) / 2;
    }

    struct cldm_rbtree tree = cldm_rbtree_init();

    for(unsigned i = 1; i < cldm_arrsize(nodes); i++) {
        ASSERT_TRUE(cldm_rbtree_insert(&tree, &nodes[i].node, compare));
    }

    ASSERT_FALSE(cldm_rbtree_remove(&tree, &nodes[0].node, compare));
    ASSERT_GE(adheres_to_rbproperties(cldm_rbroot(&tree)), 0);

    for(unsigned i = cldm_arrsize(nodes) - 1; i > 0; i--) {
        ASSERT_TRUE(cldm_rbtree_remove(&tree, &nodes[i].node, compare));
        ASSERT_GE(adheres_to_rbproperties(cldm_rbroot(&tree)), 0);
    }

    ASSERT_TRUE(cldm_rbtree_empty(&tree));
}

TEST(cldm_rbtree_remove_upper_lower) {
    struct inode nodes[SIZE];

    for(unsigned i = 0; i < cldm_arrsize(nodes); i++) {
        nodes[i].value = i - cldm_arrsize(nodes) / 2;
    }

    struct cldm_rbtree tree = cldm_rbtree_init();

    for(unsigned i = 0; i < cldm_arrsize(nodes); i++) {
        ASSERT_TRUE(cldm_rbtree_insert(&tree, &nodes[i].node, compare));
    }

    ASSERT_GE(adheres_to_rbproperties(cldm_rbroot(&tree)), 0);

    for(unsigned i = cldm_arrsize(nodes) / 2; i < cldm_arrsize(nodes); i++) {
        ASSERT_TRUE(cldm_rbtree_remove(&tree, &nodes[i].node, compare));
        ASSERT_GE(adheres_to_rbproperties(cldm_rbroot(&tree)), 0);
    }

    for(unsigned i = 0; i < cldm_arrsize(nodes) / 2; i++) {
        ASSERT_TRUE(cldm_rbtree_remove(&tree, &nodes[i].node, compare));
        ASSERT_GE(adheres_to_rbproperties(cldm_rbroot(&tree)), 0);
    }

    ASSERT_TRUE(cldm_rbtree_empty(&tree));
}

TEST(cldm_rbtree_remove_even_odd) {
    struct inode nodes[SIZE];

    for(unsigned i = 0; i < cldm_arrsize(nodes); i++) {
        nodes[i].value = i - cldm_arrsize(nodes) / 2;
    }

    struct cldm_rbtree tree = cldm_rbtree_init();

    for(unsigned i = 0; i < cldm_arrsize(nodes); i++) {
        ASSERT_TRUE(cldm_rbtree_insert(&tree, &nodes[i].node, compare));
    }

    ASSERT_GE(adheres_to_rbproperties(cldm_rbroot(&tree)), 0);

    for(unsigned i = 0; i < cldm_arrsize(nodes); i += 2) {
        ASSERT_TRUE(cldm_rbtree_remove(&tree, &nodes[i].node, compare));
        ASSERT_GE(adheres_to_rbproperties(cldm_rbroot(&tree)), 0);
    }

    for(unsigned i = 1; i < cldm_arrsize(nodes); i += 2) {
        ASSERT_TRUE(cldm_rbtree_remove(&tree, &nodes[i].node, compare));
        ASSERT_GE(adheres_to_rbproperties(cldm_rbroot(&tree)), 0);
    }

    ASSERT_TRUE(cldm_rbtree_empty(&tree));
}

TEST(cldm_rbtree_remove_root) {
    struct inode nodes[SIZE];
    struct cldm_rbnode *n;

    for(unsigned i = 0; i < cldm_arrsize(nodes); i++) {
        nodes[i].value = i - cldm_arrsize(nodes) / 2;
    }

    struct cldm_rbtree tree = cldm_rbtree_init();

    for(unsigned i = 0; i < cldm_arrsize(nodes); i++) {
        ASSERT_TRUE(cldm_rbtree_insert(&tree, &nodes[i].node, compare));
    }

    ASSERT_GE(adheres_to_rbproperties(cldm_rbroot(&tree)), 0);

    while(cldm_rbtree_size(&tree)) {
        n = cldm_rbroot(&tree);
        ASSERT_EQ(cldm_rbtree_remove(&tree, n, compare), n);
        ASSERT_GE(adheres_to_rbproperties(cldm_rbroot(&tree)), 0);
    }

    ASSERT_TRUE(cldm_rbtree_empty(&tree));

}

TEST(cldm_rbtree_for_each) {
    struct inode nodes[SIZE];
    struct inode *iiter;
    struct cldm_rbnode *rbiter;
    int ref[SIZE];
    unsigned i;

    for(i = 0; i < cldm_arrsize(nodes); i++) {
        nodes[i].value = i % 2 ? -i : i;
    }

    for(i = 0; i < cldm_arrsize(nodes) / 2; i++) {
        ref[i] = -(cldm_arrsize(nodes) - 1) + 2 * (int)i;
    }

    for(; i < cldm_arrsize(nodes); i++) {
        ref[i] = 2 * (i - cldm_arrsize(nodes) / 2);
    }

    struct cldm_rbtree tree = cldm_rbtree_init();

    cldm_for_each(iiter, nodes) {
        ASSERT_TRUE(cldm_rbtree_insert(&tree, &iiter->node, compare));
    }

    i = 0;
    cldm_rbtree_for_each(rbiter, &tree) {
        ASSERT_EQ(nodeval(rbiter), ref[i++]);
    }
}

TEST(cldm_rbtree_size) {
    struct inode nodes[SIZE];

    for(unsigned i = 0; i < cldm_arrsize(nodes); i++) {
        nodes[i].value = i - cldm_arrsize(nodes) / 2;
    }

    struct cldm_rbtree tree = cldm_rbtree_init();

    for(unsigned i = 0; i < cldm_arrsize(nodes); i++) {
        ASSERT_TRUE(cldm_rbtree_insert(&tree, &nodes[i].node, compare));
        ASSERT_EQ(cldm_rbtree_size(&tree), i + 1);
    }

    for(unsigned i = 0; i < cldm_arrsize(nodes); i++) {
        ASSERT_TRUE(cldm_rbtree_remove(&tree, &nodes[i].node, compare));
        ASSERT_EQ(cldm_rbtree_size(&tree), cldm_arrsize(nodes) - i - 1);
    }

    ASSERT_TRUE(cldm_rbtree_empty(&tree));
}
