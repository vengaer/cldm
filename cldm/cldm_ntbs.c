#include "cldm_ntbs.h"

char const *cldm_basename(char const *path);

#ifdef CLDM_HAS_AVX2
long long cldm_strscpy(char *restrict dst, char const *restrict src, unsigned long long dstsize);
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
#endif
