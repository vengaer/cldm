#include "cldm_ntbs.h"

ssize_t cldm_strscpy(char *restrict dst, char const *restrict src, size_t dstsize) {
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

int cldm_ntbscmp(char const *restrict str0, char const *restrict str1) {
    int diff = 0;
    do {
        diff = -1 * (*str0 < *str1) + (*str0 > *str1);
    } while(*str0++ && *str1++ && !diff);

    return diff;
}

int cldm_ntbsncmp(char const *restrict str0, char const *restrict str1, size_t n) {
    int diff = 0;
    do {
        diff = -1 * (*str0 < *str1) + (*str0 > *str1);
    } while(*str0++ && *str1++ && !diff && --n);
    return diff;
}
