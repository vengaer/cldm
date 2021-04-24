#include "rbtree_utils.h"

bool rbtree_is_leaf(struct cldm_rbnode const *node);
bool rbtree_is_red(struct cldm_rbnode const *node);
bool rbtree_is_black(struct cldm_rbnode const *node);
bool rbtree_is_red_safe(struct cldm_rbnode const *node, enum cldm_rbdir dir);
char const *rbtree_strcolor(struct cldm_rbnode const *node);

int rbtree_adheres_to_rbproperties(struct cldm_rbnode const *node, int(*compare)(struct cldm_rbnode const *restrict, struct cldm_rbnode const *restrict)) {
    int rval;
    struct cldm_rbnode *rnode;
    int hcontrib;
    int lheight;
    int rheight;

    if(rbtree_is_red(node) && (rbtree_is_red_safe(node, cldm_rbdir_left) || rbtree_is_red_safe(node, cldm_rbdir_right))) {
        return RBTREE_RED_VIOLATION;
    }

    if(cldm_rbnode_has_child(node, cldm_rbdir_left) && compare(node->left, node) <= 0) {
        return RBTREE_LEFT_CHILD_VIOLATION;
    }

    if(cldm_rbnode_has_child(node, cldm_rbdir_right) && compare(node, node->right) <= 0) {
        return RBTREE_RIGHT_CHILD_VIOLATION;
    }

    hcontrib = rbtree_is_black(node);

    if(!node->flags) {
        lheight = rbtree_adheres_to_rbproperties(node->left, compare);
        if(lheight < 0) {
            return lheight;
        }
        rheight = rbtree_adheres_to_rbproperties(node->right, compare);
        if(rheight < 0) {
            return rheight;
        }

        if(lheight != rheight) {
            return RBTREE_HEIGHT_VIOLATION;
        }

        return lheight + hcontrib;
    }
    else if(!rbtree_is_leaf(node)) {
        if(node->flags & CLDM_RBTHREADR) {
            rnode = node->left;
        }
        else {
            rnode = node->right;
        }

        rval = rbtree_adheres_to_rbproperties(rnode, compare);

        if(rval < 0) {
            return rval;
        }
        else if(rval > 0) {
            return RBTREE_HEIGHT_VIOLATION;
        }
    }

    return hcontrib;
}

char const *rbtree_strviolation(int errnum) {
    static char const *rbtree_violations[] = {
        "red violation",
        "left child violation",
        "right child violation",
        "height violation"
    };

    return rbtree_violations[(-1 * errnum) - 1];
}
