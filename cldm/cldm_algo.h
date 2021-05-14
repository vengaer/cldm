#ifndef CLDM_ALGO_H
#define CLDM_ALGO_H

#include <stdbool.h>
#include <stddef.h>

#include <sys/types.h>

bool cldm_is_prime(size_t num);
unsigned cldm_clp232(unsigned x);
ssize_t cldm_stable_partition(void *data, size_t elemsize, size_t nelems, bool(*predicate)(void const *));
size_t cldm_uniq(void *dst, void const *src, size_t elemsize, size_t nelems, int(*compare)(void const *restrict, void const *restrict));

#endif /* CLDM_ALGO_H */
