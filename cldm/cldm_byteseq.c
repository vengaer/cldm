#include "cldm_byteseq.h"

char const *cldm_basename(char const *path);

#ifdef CLDM_HAS_AVX2
long long cldm_strscpy(char *restrict dst, char const *restrict src, unsigned long long dstsize);
void *cldm_memcpy(void *restrict dst, void const *restrict src, unsigned long long n);
void *cldm_memset(void *dst, int v, unsigned long long n);
int cldm_memcmp(void const *s0, void const *s1, unsigned long long n);
unsigned cldm_scan_lt(char const *str, int sentinel);
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

void *cldm_memset(void *dst, int v, unsigned long long n) {
    unsigned char *d = dst;
    for(unsigned long long i = 0; i < n; i++) {
        d[i] = (unsigned char)v;
    }
    return dst;
}

int cldm_memcmp(void const *s0, void const *s1, unsigned long long n) {
    int diff = 0;
    unsigned char const *ss0 = s0;
    unsigned char const *ss1 = s1;
    for(unsigned long long i = 0; !diff && i < n; i++) {
        diff = *ss0 - *ss1;
    }
    return diff;
}

unsigned cldm_scan_lt(char const *str, int sentinel) {
    unsigned pos;
    for(pos = 0u; str[pos] >= sentinel; pos++);
    return pos;
}
#endif
