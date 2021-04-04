#ifndef CLDM_TESTREC_H
#define CLDM_TESTREC_H

#include "cldm_macro.h"
#include "cldm_rbtree.h"

#include <string.h>

/* Record representing a test */
struct cldm_testrec {
    char const *name;
    char const *file;
    /* For storing in tree */
    struct cldm_rbnode rbnode;
    /* Address of test */
    void(*handle)(void);
};

#define cldm_testrec_get2(node, qual)   \
    cldm_container(node, struct cldm_testrec, rbnode, qual)

#define cldm_testrec_get1(node) \
    cldm_testrec_get2(node,)

#define cldm_testrec_get(...)   \
    cldm_cat_expand(cldm_testrec_get, cldm_count(__VA_ARGS__))(__VA_ARGS__)

inline int cldm_testrec_compare(struct cldm_rbnode const *restrict left, struct cldm_rbnode const *restrict right) {
    struct cldm_testrec const *l = cldm_testrec_get(left, const);
    struct cldm_testrec const *r = cldm_testrec_get(right, const);
    int diff = strcmp(l->file, r->file);
    return (-1) * (diff + !diff * strcmp(l->name, r->name));
}

#endif /* CLDM_TESTREC_H */
