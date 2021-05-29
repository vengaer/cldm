#ifndef CLDM_BYTESEQ_H
#define CLDM_BYTESEQ_H

#include "cldm_config.h"

#include <errno.h>
#include <stddef.h>
#include <string.h>

#ifdef CLDM_HAS_AVX2
extern long long cldm_avx2_strscpy(char *, char const *, unsigned long long);
extern unsigned cldm_avx2_scan_lt(char const *str, int sentinel);
extern void *cldm_avx2_memcpy(void *restrict dst, void const *restrict src, unsigned long long n);
extern void *cldm_avx2_memset(void *dst, int v, unsigned long long n);
extern int cldm_avx2_memcmp(void const *s0, void const *s1, unsigned long long n);

inline long long cldm_strscpy(char *restrict dst, char const *restrict src, unsigned long long dstsize) {
    return cldm_avx2_strscpy(dst, src, dstsize);
}
inline void *cldm_memcpy(void *restrict dst, void const *restrict src, unsigned long long n) {
    return cldm_avx2_memcpy(dst, src, n);
}
inline void *cldm_memset(void *dst, int v, unsigned long long n) {
    return cldm_avx2_memset(dst, v, n);
}
inline int cldm_memcmp(void const *s0, void const *s1, unsigned long long n) {
    return cldm_avx2_memcmp(s0, s1, n);
}
inline unsigned cldm_scan_lt(char const *str, int sentinel) {
    return cldm_avx2_scan_lt(str, sentinel);
}
#else
long long cldm_strscpy(char *restrict dst, char const *restrict src, unsigned long long dstsize);
void *cldm_memcpy(void *restrict dst, void const *src, unsigned long long n);
void *cldm_memset(void *dst, int v, unsigned long long n);
int cldm_memcmp(void const *s0, void const *s1, unsigned long long n);
unsigned cldm_scan_lt(char const *str, int sentinel);
#endif

inline char const *cldm_basename(char const *path) {
    char const *p = strrchr(path, '/');
    return p ? p + 1 : path;
}


#endif /* CLDM_BYTESEQ_H */
