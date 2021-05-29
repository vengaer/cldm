#ifndef CLDM_BYTESEQ_H
#define CLDM_BYTESEQ_H

#include "cldm_config.h"

#include <errno.h>
#include <stddef.h>
#include <string.h>

#ifdef CLDM_HAS_AVX2
extern long long cldm_avx2_strscpy(char *, char const *, unsigned long long);
extern void *cldm_avx2_memcpy(void *restrict dst, void const *restrict src, unsigned long long n);

inline long long cldm_strscpy(char *restrict dst, char const *restrict src, unsigned long long dstsize) {
    return cldm_avx2_strscpy(dst, src, dstsize);
}
inline void *cldm_memcpy(void *restrict dst, void const *restrict src, unsigned long long n) {
    return cldm_avx2_memcpy(dst, src, n);
}
#else
long long cldm_strscpy(char *restrict dst, char const *restrict src, unsigned long long dstsize);
void *cldm_memcpy(void *restrict dst, void const *src, unsigned long long n);
#endif

inline char const *cldm_basename(char const *path) {
    char const *p = strrchr(path, '/');
    return p ? p + 1 : path;
}

inline unsigned cldm_scan_lt(char const *str, int lim) {
    unsigned pos;
    for(pos = 0u; str[pos] > lim; pos++);
    return pos;
}

#endif /* CLDM_BYTESEQ_H */
