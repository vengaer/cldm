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

char const *cldm_ntbs_find_substr(char const *restrict str, char const *restrict substr) {
    char const *d = substr;

    for(;*str && *d; ++str) {
        d = *str == *d ? d + 1 : substr;
    }

    return *d ? 0 : str - strlen(substr);
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
