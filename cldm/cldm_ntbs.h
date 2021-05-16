#ifndef CLDM_NTBS_H
#define CLDM_NTBS_H

#include "cldm_config.h"

#include <errno.h>
#include <stddef.h>
#include <string.h>

#ifdef CLDM_HAS_AVX2
inline long long cldm_strscpy(char *restrict dst, char const *restrict src, unsigned long long dstsize) {
    extern long long cldm_avx2_strscpy(char *, char const *, unsigned long long);
    return cldm_avx2_strscpy(dst, src, dstsize);
}
#else
long long cldm_strscpy(char *restrict dst, char const *restrict src, unsigned long long dstsize);
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

#endif /* CLDM_NTBS_H */
