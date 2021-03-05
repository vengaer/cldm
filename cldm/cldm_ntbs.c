#include "cldm_ntbs.h"

char *cldm_ntbschr(char *str, int c) {
    while(*str != (char)c && *str++);
    return *str ? str : 0;
}

size_t cldm_ntbslen(char const *str) {
    size_t len = 0;
    while(*str++) {
        ++len;
    }
    return len;
}

ssize_t cldm_ntbscpy(char *restrict dst, char const *restrict src, size_t dstsize) {
    size_t const srclen = cldm_ntbslen(src);
    size_t i = dstsize;
    char *d = dst;

    while(i-- && (*d++ = *src++));

    if(srclen >= dstsize) {
        dst[dstsize - 1] = '\0';
        return -E2BIG;
    }

    return srclen;
}
