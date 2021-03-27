#ifndef CLDM_TESTREC_H
#define CLDM_TESTREC_H

#include "cldm_macro.h"
#include "cldm_ntbs.h"
#include "cldm_rbtree.h"

struct cldm_testrec {
    char const *name;
    char const *file;
    struct cldm_rbnode rbnode;
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
    int diff = cldm_ntbscmp(l->file, r->file);
    return (-1) * (diff + !diff * cldm_ntbscmp(l->name, r->name));
}

#endif /* CLDM_TESTREC_H */
