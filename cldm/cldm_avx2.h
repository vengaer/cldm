#ifndef CLDM_AVX2_H
#define CLDM_AVX2_H

#include "cldm_config.h"

#ifdef CLDM_HAS_AVX2

extern long long cldm_avx2_strscpy(char *restrict dst, char const *restrict src, unsigned long long dstsize);
extern void *cldm_avx2_memset(void *dst, int v, unsigned long long n);
extern void *cldm_avx2_memcpy(void *restrict dst, void const *restrict src, unsigned long long n);
extern int cldm_avx2_memcmp(void const *s0, void const *s1, unsigned long long n);
extern unsigned cldm_avx2_scan_lt(char const *str, int sentinel);
extern unsigned long long cldm_avx2_strlen(char const *str);
extern void cldm_avx2_memswp(void *restrict s0, void *restrict s1, unsigned long long n);
extern int cldm_avx2_strcmp(char const *str0, char const *str1);
extern char *cldm_avx2_strrchr(char const *str, int c);

#endif

#endif /* CLDM_AVX2_H */
