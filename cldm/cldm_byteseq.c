#include "cldm_byteseq.h"

char const *cldm_basename(char const *path);
unsigned cldm_scan_lt(char const *str, int lim);

#ifdef CLDM_HAS_AVX2
long long cldm_strscpy(char *restrict dst, char const *restrict src, unsigned long long dstsize);
void *cldm_memcpy(void *restrict dst, void const *restrict src, unsigned long long n);

#else

long long cldm_strscpy(char *restrict dst, char const *restrict src, unsigned long long dstsize) {
    size_t const srclen = strlen(src);
    size_t i = dstsize;
    char *d = dst;

    while(i-- && (*d++ = *src++));

    if(srclen >= dstsize) {
        dst[dstsize - 1] = '\0';
        return -E2BIG;
    }

    return srclen;
}

void *cldm_memcpy(void *restrict dst, void const *restrict src, unsigned long long n) {
    unsigned char *d = dst;
    unsigned char const *s = src;
    for(unsigned long long i = 0; i < n; i++) {
        d[i] = s[i];
    }
    return dst;
}
#endif
