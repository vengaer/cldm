#include "cldm_ntbs.h"

char const *cldm_basename(char const *path);

long long cldm_strscpy(char *restrict dst, char const *restrict src, size_t dstsize) {
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
