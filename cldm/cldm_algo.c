#include "cldm_algo.h"
#include "cldm_log.h"

#include <stdlib.h>
#include <string.h>

bool cldm_is_prime(size_t num) {
    if(num == 2 || num == 3) {
        return true;
    }
    if((!num % 2) || !(num % 3)) {
        return false;
    }

    /* Remaining primes are on form 6n +/- 1 */
    for(size_t div = 6u; div * div - 2 * div < num; div += 6u) {
        if(!(num % (div - 1)) || !(num % div + 1)) {
            return false;
        }
    }

    return true;
}

ssize_t cldm_stable_partition(void *data, size_t elemsize, size_t nelems, bool(*predicate)(void const *)) {
    unsigned char *arr;
    unsigned char *elem;
    unsigned st = 0u;
    unsigned ust = 0u;

    arr = malloc(2 * nelems * elemsize);

    if(!arr) {
        cldm_err("Could not allocate memory for partitioning");
        return -1;
    }

    for(unsigned i = 0; i < nelems; i++) {
        elem = (unsigned char *)data + i * elemsize;
        if(predicate(elem)) {
            /* Write to lower half */
            memcpy(arr + elemsize * st++, elem, elemsize);
        }
        else {
            /* Write to upper half */
            memcpy(arr + (nelems + ust++) * elemsize, elem, elemsize);
        }
    }

    /* Write back */
    memcpy(data, arr, st * elemsize);
    memcpy((unsigned char *)data + st * elemsize, arr + nelems * elemsize, ust * elemsize);

    free(arr);

    return st;
}

size_t cldm_uniq(void *dst, void const *src, size_t elemsize, size_t nelems, int(*compare)(void const *restrict, void const *restrict)) {
    unsigned char const *prev = src;
    unsigned char const *curr = (unsigned char const *)src + elemsize;
    unsigned char *d = (unsigned char *)dst + elemsize;

    memmove(dst, src, elemsize);

    for(unsigned i = 0; i < nelems - 1; i++) {
        if(compare(prev, curr)) {
            memmove(d, curr, elemsize);
            d += elemsize;
            prev = curr;
        }
        curr += elemsize;
    }

    return (d - (unsigned char *)dst) / elemsize;
}
