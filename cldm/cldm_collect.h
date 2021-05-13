#ifndef CLDM_COLLECT_H
#define CLDM_COLLECT_H

#include "cldm_elf.h"
#include "cldm_rbtree.h"

#include <stddef.h>

#include <sys/types.h>

ssize_t cldm_collect(struct cldm_rbtree *restrict tree, struct cldm_elfmap const *restrict map);
ssize_t cldm_collect_from(struct cldm_rbtree *restrict tree, struct cldm_elfmap const *restrict map, char *const *restrict files, size_t nfiles);

#endif /* CLDM_COLLECT_H */
